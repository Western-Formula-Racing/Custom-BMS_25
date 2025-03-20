# main.py
import tkinter as tk
import random
import matplotlib
import queue
from multiprocessing import Process, Queue
import can_listener

matplotlib.use("Agg")  # We won't use an embedded Matplotlib window for this approach

# Material Design Color Palette
BG_COLOR = "#000000"          # Main background - black
SECONDARY_BG = "#212121"      # Secondary background - dark gray
CELL_BG = "#424242"           # Cell background - medium dark gray
ACCENT_COLOR = "#ff9800"      # Accent color - material orange
TEXT_COLOR = "#ffffff"        # Text color - white

# Global threshold variables
VOLTAGE_HIGH_THRESHOLD = 3.7
VOLTAGE_LOW_THRESHOLD = 2.5
TEMP_HIGH_THRESHOLD = 60
TEMP_LOW_THRESHOLD = 0

CURRENT_HIGH_THRESHOLD = 100
CURRENT_LOW_THRESHOLD = 0
RESISTOR_TEMP_HIGH_THRESHOLD = 60
RESISTOR_TEMP_LOW_THRESHOLD = 0

FLASH_COLOR = ACCENT_COLOR  # Using accent color for flashing warnings

# Global queue for CAN messages.
# (This queue is created in main.py and shared with the CAN listener process.)
can_queue = Queue()


def coolwarm_color(value, low, high):
    """
    Returns a hex color string using a coolwarm gradient.
    For values at or near 'low', the color is blue; at the midpoint, white;
    and at or near 'high', red.
    """
    if high == low:
        norm = 0.5
    else:
        norm = (value - low) / (high - low)
    norm = max(0.0, min(1.0, norm))
    if norm <= 0.5:
        ratio = norm / 0.5
        r = int(255 * ratio)
        g = int(255 * ratio)
        b = 255
    else:
        ratio = (norm - 0.5) / 0.5
        r = 255
        g = int(255 * (1 - ratio))
        b = int(255 * (1 - ratio))
    return f"#{r:02x}{g:02x}{b:02x}"


def draw_sparkline(canvas, data, min_val, max_val):
    """
    Draws a small sparkline on the given canvas using the values in `data`.
    The sparkline is drawn in a 50%-opaque gray over the canvas.
    Additionally, the most recent raw value is overlaid in the center.
    """
    canvas.delete("all")  # clear previous drawing

    width = int(canvas.cget("width"))
    height = int(canvas.cget("height"))

    if len(data) < 2:
        if data:
            raw_value = data[-1]
            if hasattr(canvas, "sensor_type") and canvas.sensor_type == "voltage":
                text = f"{raw_value:.3f}"
            elif hasattr(canvas, "sensor_type") and canvas.sensor_type == "temperature":
                text = f"{raw_value}°C"
            elif hasattr(canvas, "sensor_type") and canvas.sensor_type == "current":
                text = f"{raw_value:.1f}"
            elif hasattr(canvas, "sensor_type") and canvas.sensor_type == "resistor":
                text = f"{raw_value}°C"
            else:
                text = str(raw_value)
            canvas.create_text(width // 2, height // 2, text=text, fill=TEXT_COLOR)
        return

    points = []
    n = len(data)
    step_x = width / (n - 1) if n > 1 else width

    for i, val in enumerate(data):
        x = i * step_x
        if val < min_val:
            val = min_val
        if val > max_val:
            val = max_val
        ratio = (val - min_val) / (max_val - min_val) if (max_val > min_val) else 0.5
        y = height - ratio * height
        points.append(x)
        points.append(y)

    canvas.create_line(points, fill="#7F7F7F", width=1.5)
    raw_value = data[-1]
    if hasattr(canvas, "sensor_type") and canvas.sensor_type == "voltage":
        text = f"{raw_value:.3f}"
    elif hasattr(canvas, "sensor_type") and canvas.sensor_type == "temperature":
        text = f"{raw_value}°C"
    elif hasattr(canvas, "sensor_type") and canvas.sensor_type == "current":
        text = f"{raw_value:.1f} mA"
    elif hasattr(canvas, "sensor_type") and canvas.sensor_type == "resistor":
        text = f"{raw_value}°C"
    else:
        text = str(raw_value)
    canvas.create_text(width // 2, height // 2, text=text, fill=TEXT_COLOR)


class BMSSimulatorGUI:
    def __init__(self, root, can_queue):
        self.root = root
        self.can_queue = can_queue  # Use the shared CAN queue
        self.root.title("BMS CAN Messaging GUI")
        self.root.configure(bg=BG_COLOR)

        # Global total voltage label at the top
        self.global_total_voltage_label = tk.Label(root, text="Global Total Voltage: 0.000", bg=BG_COLOR, fg=ACCENT_COLOR)
        self.global_total_voltage_label.pack(pady=5)

        # Create a scrollable container
        container = tk.Frame(root, bg=BG_COLOR)
        container.pack(fill="both", expand=True)

        self.canvas = tk.Canvas(container, bg=BG_COLOR)
        self.canvas.pack(side="left", fill="both", expand=True)

        scrollbar = tk.Scrollbar(container, orient="vertical", command=self.canvas.yview)
        scrollbar.pack(side="right", fill="y")
        self.canvas.configure(yscrollcommand=scrollbar.set)

        # This frame goes inside the canvas
        self.scrollable_frame = tk.Frame(self.canvas, bg=BG_COLOR)
        self.scrollable_frame.bind("<Configure>", lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all")))
        self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")

        # Data structures for the grids
        self.voltage_frames = []  # For Voltage grid (modules M1-M5)
        self.temp_frames = []     # For Temperature grid (modules M1-M5)
        self.current_frames = []  # For Balancing Current grid (modules M1-M5)
        self.voltage_canvases = []
        self.temp_canvases = []
        self.current_canvases = []
        self.voltage_history = [[[[] for _ in range(5)] for _ in range(4)] for _ in range(5)]
        self.temp_history = [[[[] for _ in range(6)] for _ in range(3)] for _ in range(5)]
        self.current_history = [[[[] for _ in range(5)] for _ in range(4)] for _ in range(5)]

        # For Resistor Temperature grids (only for modules M1 and M2)
        self.resistor_frames_global = []  # Two sets: index 0 for M1, 1 for M2
        self.resistor_canvases_global = []
        self.resistor_history_global = []

        # Dictionary for flashing frames
        self.flashing_frames = {}
        self.module_frames = []  # Module header frames

        # Build grids for modules M1-M5 (Voltage, Temperature, Balancing Current)
        for module in range(5):
            module_frame = tk.LabelFrame(self.scrollable_frame,
                                         text=f"Module {module + 1} - Total Voltage: 0.000 - Avg Temp: 0.0°C",
                                         bg=SECONDARY_BG, fg=ACCENT_COLOR, padx=5, pady=5)
            module_frame.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
            self.module_frames.append(module_frame)

            mod_volt_frames = []
            mod_volt_canvases = []
            mod_temp_frames = []
            mod_temp_canvases = []
            mod_current_frames = []
            mod_current_canvases = []

            # Voltage grid: 5 columns x 4 rows (cells labeled V1 ... V20)
            voltage_frame = tk.LabelFrame(module_frame, text="Voltage (5x4 grid, V)",
                                          bg=SECONDARY_BG, fg=ACCENT_COLOR, padx=5, pady=5)
            voltage_frame.grid(row=0, column=0, padx=5, pady=5)
            volt_counter = 1
            for row in range(4):
                row_frames = []
                row_canvases = []
                for col in range(5):
                    cell_frame = tk.Frame(voltage_frame, width=80, height=30, bg=CELL_BG,
                                          relief=tk.RIDGE, borderwidth=2)
                    cell_frame.grid_propagate(False)
                    cell_frame.grid(row=row, column=col, padx=2, pady=2)
                    c = tk.Canvas(cell_frame, width=80, height=30, bg=BG_COLOR, highlightthickness=0)
                    c.pack(fill="both", expand=True)
                    tag = f"V{volt_counter}"
                    label = tk.Label(cell_frame, text=tag, bg=CELL_BG, fg=TEXT_COLOR, font=("Arial", 8))
                    label.place(x=2, y=2)
                    volt_counter += 1
                    c.sensor_type = "voltage"
                    c.module = module
                    c.row = row
                    c.col = col
                    row_frames.append(cell_frame)
                    row_canvases.append(c)
                mod_volt_frames.append(row_frames)
                mod_volt_canvases.append(row_canvases)
            self.voltage_frames.append(mod_volt_frames)
            self.voltage_canvases.append(mod_volt_canvases)

            # Temperature grid: 6 columns x 3 rows (cells labeled T1 ... T18)
            temp_frame = tk.LabelFrame(module_frame, text="Temperature (6x3 grid)",
                                       bg=SECONDARY_BG, fg=ACCENT_COLOR, padx=5, pady=5)
            temp_frame.grid(row=0, column=1, padx=5, pady=5)
            temp_counter = 1
            for row in range(3):
                row_frames = []
                row_canvases = []
                for col in range(6):
                    cell_frame = tk.Frame(temp_frame, width=80, height=30, bg=CELL_BG,
                                          relief=tk.RIDGE, borderwidth=2)
                    cell_frame.grid_propagate(False)
                    cell_frame.grid(row=row, column=col, padx=2, pady=2)
                    c = tk.Canvas(cell_frame, width=80, height=30, bg=BG_COLOR, highlightthickness=0)
                    c.pack(fill="both", expand=True)
                    tag = f"T{temp_counter}"
                    label = tk.Label(cell_frame, text=tag, bg=CELL_BG, fg=TEXT_COLOR, font=("Arial", 8))
                    label.place(x=2, y=2)
                    temp_counter += 1
                    c.sensor_type = "temperature"
                    c.module = module
                    c.row = row
                    c.col = col
                    row_frames.append(cell_frame)
                    row_canvases.append(c)
                mod_temp_frames.append(row_frames)
                mod_temp_canvases.append(row_canvases)
            self.temp_frames.append(mod_temp_frames)
            self.temp_canvases.append(mod_temp_canvases)

            # Balancing Current grid: 5 columns x 4 rows (cells labeled C1 ... C20)
            current_frame = tk.LabelFrame(module_frame, text="Balancing Current (5x4 grid, mA)",
                                          bg=SECONDARY_BG, fg=ACCENT_COLOR, padx=5, pady=5)
            current_frame.grid(row=1, column=0, padx=5, pady=5)
            current_counter = 1
            for row in range(4):
                row_frames = []
                row_canvases = []
                for col in range(5):
                    cell_frame = tk.Frame(current_frame, width=80, height=30, bg=CELL_BG,
                                          relief=tk.RIDGE, borderwidth=2)
                    cell_frame.grid_propagate(False)
                    cell_frame.grid(row=row, column=col, padx=2, pady=2)
                    c = tk.Canvas(cell_frame, width=80, height=30, bg=BG_COLOR, highlightthickness=0)
                    c.pack(fill="both", expand=True)
                    tag = f"C{current_counter}"
                    label = tk.Label(cell_frame, text=tag, bg=CELL_BG, fg=TEXT_COLOR, font=("Arial", 8))
                    label.place(x=2, y=2)
                    current_counter += 1
                    c.sensor_type = "current"
                    c.module = module
                    c.row = row
                    c.col = col
                    row_frames.append(cell_frame)
                    row_canvases.append(c)
                mod_current_frames.append(row_frames)
                mod_current_canvases.append(row_canvases)
            self.current_frames.append(mod_current_frames)
            self.current_canvases.append(mod_current_canvases)

        # Global Resistor Temperature grids (only for modules M1 and M2)
        resistor_global_frame = tk.LabelFrame(self.scrollable_frame, text="Resistor Temperature (2 sets)",
                                              bg=SECONDARY_BG, fg=ACCENT_COLOR, padx=5, pady=5)
        resistor_global_frame.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
        valid_tags = ["R1", "R2", "R3", "R4", "R5", "R7", "R8", "R9", "R10", "R11", "R12",
                      "R13", "R14", "R15", "R17", "R18", "R19", "R20"]
        rows = 3
        cols = 6
        for set_index in range(2):  # For M1 and M2
            set_frame = tk.LabelFrame(resistor_global_frame, text=f"Resistor Set {set_index + 1}",
                                      bg=SECONDARY_BG, fg=ACCENT_COLOR, padx=5, pady=5)
            set_frame.pack(side=tk.LEFT, padx=10, pady=5)
            set_frames = []
            set_canvases = []
            set_history = []
            resistor_counter = 0
            for row in range(rows):
                row_frames = []
                row_canvases = []
                row_history = []
                for col in range(cols):
                    tag = valid_tags[resistor_counter]
                    cell_frame = tk.Frame(set_frame, width=80, height=30, bg=CELL_BG,
                                          relief=tk.RIDGE, borderwidth=2)
                    cell_frame.grid_propagate(False)
                    cell_frame.grid(row=row, column=col, padx=2, pady=2)
                    c = tk.Canvas(cell_frame, width=80, height=30, bg=BG_COLOR, highlightthickness=0)
                    c.pack(fill="both", expand=True)
                    label = tk.Label(cell_frame, text=tag, bg=CELL_BG, fg=TEXT_COLOR, font=("Arial", 8))
                    label.place(x=2, y=2)
                    c.sensor_type = "resistor"
                    c.module = set_index  # 0 for M1, 1 for M2
                    c.row = row
                    c.col = col
                    row_frames.append(cell_frame)
                    row_canvases.append(c)
                    row_history.append([])
                    resistor_counter += 1
                set_frames.append(row_frames)
                set_canvases.append(row_canvases)
                set_history.append(row_history)
            self.resistor_frames_global.append(set_frames)
            self.resistor_canvases_global.append(set_canvases)
            self.resistor_history_global.append(set_history)

        # Start polling the CAN message queue
        self.poll_can_messages()

    def flash_toggle(self, frame):
        """Toggle flashing for a frame."""
        if frame not in self.flashing_frames:
            return
        flash_info = self.flashing_frames[frame]
        current_state = flash_info["state"]
        base_color = flash_info["base_color"]
        new_color = FLASH_COLOR if current_state else base_color
        frame.config(bg=new_color)
        flash_info["state"] = not current_state
        flash_info["job"] = self.root.after(500, lambda: self.flash_toggle(frame))

    def start_flashing(self, frame, base_color):
        """Begin flashing for the given frame."""
        if frame in self.flashing_frames:
            self.flashing_frames[frame]["base_color"] = base_color
            return
        self.flashing_frames[frame] = {"state": True, "base_color": base_color, "job": None}
        self.flash_toggle(frame)

    def stop_flashing(self, frame, base_color):
        """Stop flashing for the given frame."""
        if frame in self.flashing_frames:
            job = self.flashing_frames[frame].get("job")
            if job:
                self.root.after_cancel(job)
            frame.config(bg=base_color)
            del self.flashing_frames[frame]

    def update_from_can_message(self, msg):
        """
        Process a CAN message tuple of the form:
            (module, cell, reading)
        module: e.g., "M1", "M2", ..., "M5" (for Voltage, Temperature, Current)
                For resistor readings, only "M1" or "M2" are valid.
        cell: e.g., "V1", "T1", "C1", "R1", etc.
        reading: numeric value
        """
        mod_str, cell_str, reading = msg
        # For non-resistor cells, module index is int(mod_str[1:]) - 1.
        if cell_str.startswith("V"):
            module_index = int(mod_str[1:]) - 1
            cell_index = int(cell_str[1:]) - 1  # 0-indexed
            row = cell_index // 5
            col = cell_index % 5
            hist = self.voltage_history[module_index][row][col]
            hist.append(reading)
            if len(hist) > 60:
                hist.pop(0)
            base_color = coolwarm_color(reading, VOLTAGE_LOW_THRESHOLD, VOLTAGE_HIGH_THRESHOLD)
            frame = self.voltage_frames[module_index][row][col]
            if reading < VOLTAGE_LOW_THRESHOLD or reading > VOLTAGE_HIGH_THRESHOLD:
                self.start_flashing(frame, base_color)
            else:
                self.stop_flashing(frame, base_color)
                frame.config(bg=base_color)
            canvas = self.voltage_canvases[module_index][row][col]
            draw_sparkline(canvas, hist, VOLTAGE_LOW_THRESHOLD, VOLTAGE_HIGH_THRESHOLD)
        elif cell_str.startswith("T"):
            module_index = int(mod_str[1:]) - 1
            cell_index = int(cell_str[1:]) - 1
            row = cell_index // 6
            col = cell_index % 6
            hist = self.temp_history[module_index][row][col]
            hist.append(reading)
            if len(hist) > 60:
                hist.pop(0)
            base_color = coolwarm_color(reading, TEMP_LOW_THRESHOLD, TEMP_HIGH_THRESHOLD)
            frame = self.temp_frames[module_index][row][col]
            if reading < TEMP_LOW_THRESHOLD or reading > TEMP_HIGH_THRESHOLD:
                self.start_flashing(frame, base_color)
            else:
                self.stop_flashing(frame, base_color)
                frame.config(bg=base_color)
            canvas = self.temp_canvases[module_index][row][col]
            draw_sparkline(canvas, hist, TEMP_LOW_THRESHOLD, TEMP_HIGH_THRESHOLD)
        elif cell_str.startswith("C"):
            module_index = int(mod_str[1:]) - 1
            cell_index = int(cell_str[1:]) - 1
            row = cell_index // 5
            col = cell_index % 5
            hist = self.current_history[module_index][row][col]
            hist.append(reading)
            if len(hist) > 60:
                hist.pop(0)
            base_color = coolwarm_color(reading, CURRENT_LOW_THRESHOLD, CURRENT_HIGH_THRESHOLD)
            frame = self.current_frames[module_index][row][col]
            if reading < CURRENT_LOW_THRESHOLD or reading > CURRENT_HIGH_THRESHOLD:
                self.start_flashing(frame, base_color)
            else:
                self.stop_flashing(frame, base_color)
                frame.config(bg=base_color)
            canvas = self.current_canvases[module_index][row][col]
            draw_sparkline(canvas, hist, CURRENT_LOW_THRESHOLD, CURRENT_HIGH_THRESHOLD)
        elif cell_str.startswith("R"):
            # For resistor readings, we expect module to be "M1" or "M2"
            module_index = int(mod_str[1:]) - 1  # should be 0 or 1
            valid_tags = ["R1", "R2", "R3", "R4", "R5", "R7", "R8", "R9", "R10",
                          "R11", "R12", "R13", "R14", "R15", "R17", "R18", "R19", "R20"]
            try:
                cell_index = valid_tags.index(cell_str)
            except ValueError:
                return  # invalid cell tag
            row = cell_index // 6
            col = cell_index % 6
            hist = self.resistor_history_global[module_index][row][col]
            hist.append(reading)
            if len(hist) > 60:
                hist.pop(0)
            base_color = coolwarm_color(reading, RESISTOR_TEMP_LOW_THRESHOLD, RESISTOR_TEMP_HIGH_THRESHOLD)
            frame = self.resistor_frames_global[module_index][row][col]
            if reading < RESISTOR_TEMP_LOW_THRESHOLD or reading > RESISTOR_TEMP_HIGH_THRESHOLD:
                self.start_flashing(frame, base_color)
            else:
                self.stop_flashing(frame, base_color)
                frame.config(bg=base_color)
            canvas = self.resistor_canvases_global[module_index][row][col]
            draw_sparkline(canvas, hist, RESISTOR_TEMP_LOW_THRESHOLD, RESISTOR_TEMP_HIGH_THRESHOLD)

    def poll_can_messages(self):
        """
        Poll the global can_queue for new CAN messages and process them.
        This method schedules itself to run repeatedly.
        """
        while not self.can_queue.empty():
            msg = self.can_queue.get()
            self.update_from_can_message(msg)
        self.root.after(100, self.poll_can_messages)


if __name__ == "__main__":
    from multiprocessing import Process, Queue
    # Start the CAN listener process with the shared queue
    listener_process = Process(target=can_listener.run_can_listener, args=(can_queue,))
    listener_process.daemon = True  # Allow the program to exit if the GUI is closed
    listener_process.start()

    root = tk.Tk()
    app = BMSSimulatorGUI(root, can_queue)
    root.mainloop()