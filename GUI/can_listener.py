import can
import os
import cantools
import datetime
import random, time

# Convert raw CAN signal names to GUI cell tags
lookup = {
    # Voltage signals from DBC (per module)
    "Cell1_Voltage": "V1", "Cell2_Voltage": "V2",
    "Cell3_Voltage": "V3", "Cell4_Voltage": "V4",
    "Cell5_Voltage": "V5", "Cell6_Voltage": "V6",
    "Cell7_Voltage": "V7", "Cell8_Voltage": "V8",
    "Cell9_Voltage": "V9", "Cell10_Voltage": "V10",
    "Cell11_Voltage": "V11", "Cell12_Voltage": "V12",
    "Cell13_Voltage": "V13", "Cell14_Voltage": "V14",
    "Cell15_Voltage": "V15", "Cell16_Voltage": "V16",
    "Cell17_Voltage": "V17", "Cell18_Voltage": "V18",
    "Cell19_Voltage": "V19", "Cell20_Voltage": "V20",
    # Temperature (thermistor) signals
    "Thermistor1": "T1", "Thermistor2": "T2",
    "Thermistor3": "T3", "Thermistor4": "T4",
    "Thermistor5": "T5", "Thermistor6": "T6",
    "Thermistor7": "T7", "Thermistor8": "T8",
    "Thermistor9": "T9", "Thermistor10": "T10",
    "Thermistor11": "T11", "Thermistor12": "T12",
    "Thermistor13": "T13", "Thermistor14": "T14",
    "Thermistor15": "T15", "Thermistor16": "T16",
    "Thermistor17": "T17", "Thermistor18": "T18",
    # Balancing current signals
    "Current1": "C1", "Current2": "C2",
    "Current3": "C3", "Current4": "C4",
    "Current5": "C5", "Current6": "C6",
    "Current7": "C7", "Current8": "C8",
    "Current9": "C9", "Current10": "C10",
    "Current11": "C11", "Current12": "C12",
    "Current13": "C13", "Current14": "C14",
    "Current15": "C15", "Current16": "C16",
    "Current17": "C17", "Current18": "C18",
    "Current19": "C19", "Current20": "C20",
    # Resistor temperature signals (for M1 and M2)
    "Resistor1": "R1", "Resistor2": "R2",
    "Resistor3": "R3", "Resistor4": "R4",
    "Resistor5": "R5", "Resistor7": "R7",
    "Resistor8": "R8", "Resistor9": "R9",
    "Resistor10": "R10", "Resistor11": "R11",
    "Resistor12": "R12", "Resistor13": "R13",
    "Resistor14": "R14", "Resistor15": "R15",
    "Resistor17": "R17", "Resistor18": "R18",
    "Resistor19": "R19", "Resistor20": "R20",
}

# Load the DBC for real CAN decoding
db = cantools.database.load_file("WFR25.dbc")

def simulate_can_messages(can_queue, modules, voltage_cells, temp_cells, current_cells, resistor_cells):
    while True:
        msg_type = random.choice(["voltage", "temperature", "current", "resistor"])
        if msg_type == "voltage":
            mod = random.choice(modules)
            cell = random.choice(voltage_cells)
            reading = round(random.uniform(2.9, 3.9), 3)
        elif msg_type == "temperature":
            mod = random.choice(modules)
            cell = random.choice(temp_cells)
            reading = random.randint(5, 45)
        elif msg_type == "current":
            mod = random.choice(modules)
            cell = random.choice(current_cells)
            reading = round(random.uniform(0, 100), 1)
        else:  # resistor
            mod = random.choice(["M1", "M2"])
            cell = random.choice(resistor_cells)
            reading = random.randint(10, 50)
        can_queue.put((mod, cell, reading))
        time.sleep(0.01)

def run_can_listener(can_queue, simulate=True):
    """
    Starts either a simulation loop or real CAN decoding.
    """
    voltage_cells = [f"V{i}" for i in range(1, 21)]
    temp_cells = [f"T{i}" for i in range(1, 19)]
    current_cells = [f"C{i}" for i in range(1, 21)]
    resistor_cells = ["R" + str(i) for i in [1,2,3,4,5,7,8,9,10,11,12,13,14,15,17,18,19,20]]
    modules = ["M1", "M2", "M3", "M4", "M5"]

    if simulate:
        simulate_can_messages(can_queue, modules, voltage_cells, temp_cells, current_cells, resistor_cells)
    else:
        bus = can.interface.Bus(channel='can0', bustype='kvaser')
        while True:
            msg = bus.recv()
            if msg is None:
                continue
            try:
                message_def = db.get_message_by_frame_id(msg.arbitration_id)
            except KeyError:
                continue
            if "TORCH" not in message_def.name or "STAT" in message_def.name:
                continue
            decoded = message_def.decode(msg.data)
            for signal_name, value in decoded.items():
                parts = signal_name.split('_')
                module = parts[0]
                cell_raw = '_'.join(parts[1:])
                cell_tag = lookup.get(cell_raw)
                timestamp = datetime.datetime.now()
                can_queue.put((module, cell_tag, value, timestamp))

if __name__ == '__main__':
    # For standalone testing
    from multiprocessing import Queue

    q = Queue()
    run_can_listener(q, simulate=False)