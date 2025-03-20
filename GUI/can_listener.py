# can_listener.py
import random
import time


def run_can_listener(can_queue):
    """
    Simulates reading from the CAN bus and processing messages.

    Data Communication Protocol:
      Each message is a tuple:
         (module, cell, reading)
      where:
         - module: string, one of "M1", "M2", "M3", "M4", "M5" (for Voltage, Temperature, Current)
                   For resistor readings, only "M1" or "M2" are used.
         - cell: string representing the sensor identifier (e.g., "V1", "T1", "C1", "R1", etc.)
         - reading: numeric value representing the sensor reading.
    """
    voltage_cells = [f"V{i}" for i in range(1, 21)]
    temp_cells = [f"T{i}" for i in range(1, 19)]
    current_cells = [f"C{i}" for i in range(1, 21)]
    resistor_cells = ["R1", "R2", "R3", "R4", "R5", "R7", "R8", "R9", "R10",
                      "R11", "R12", "R13", "R14", "R15", "R17", "R18", "R19", "R20"]
    modules = ["M1", "M2", "M3", "M4", "M5"]

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


if __name__ == '__main__':
    # For standalone testing
    from multiprocessing import Queue

    q = Queue()
    run_can_listener(q)