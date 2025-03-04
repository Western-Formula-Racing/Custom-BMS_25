import tkinter as tk
import random
import matplotlib

matplotlib.use("Agg")  # We won't use an embedded Matplotlib window for this approach

# Global threshold variables
VOLTAGE_HIGH_THRESHOLD = 4
VOLTAGE_LOW_THRESHOLD = 2.5
TEMP_HIGH_THRESHOLD = 60
TEMP_LOW_THRESHOLD = 0

FLASH_COLOR = "yellow"  # color used for flashing warning


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
    The sparkline is drawn in a 50%-opaque white (#7F7F7F) over the black canvas.
    Additionally, the most recent raw value is overlaid in the center.
    """
    canvas.delete("all")  # clear previous drawing

    width = int(canvas.cget("width"))
    height = int(canvas.cget("height"))

    # If there's not enough data for a line, still display the raw value.
    if len(data) < 2:
        if data:
            raw_value = data[-1]
            text = f"{raw_value:.3f}" if (
                        hasattr(canvas, "sensor_type") and canvas.sensor_type == "voltage") else f"{raw_value}°C"
            canvas.create_text(width // 2, height // 2, text=text, fill="white")
        return

    points = []
    n = len(data)
    step_x = width / (n - 1) if n > 1 else width

    for i, val in enumerate(data):
        x = i * step_x
        # Clamp val between min_val and max_val
        if val < min_val:
            val = min_val
        if val > max_val:
            val = max_val
        ratio = (val - min_val) / (max_val - min_val) if (max_val > min_val) else 0.5
        y = height - ratio * height
        points.append(x)
        points.append(y)

    # Draw the sparkline with 50% transparency (simulated using #7F7F7F)
    canvas.create_line(points, fill="#7F7F7F", width=1.5)

    # Draw the raw value in the center
    raw_value = data[-1]
    if hasattr(canvas, "sensor_type") and canvas.sensor_type == "voltage":
        text = f"{raw_value:.3f}"
    else:
        text = f"{raw_value}°C"
    canvas.create_text(width // 2, height // 2, text=text, fill="white")


class BMSSimulatorGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("BMS CAN Messaging GUI")

        # Global total voltage label at the top
        self.global_total_voltage_label = tk.Label(root, text="Global Total Voltage: 0.000")
        self.global_total_voltage_label.pack(pady=5)

        # -- Create a scrollable container ------------------------------------
        container = tk.Frame(root)
        container.pack(fill="both", expand=True)

        self.canvas = tk.Canvas(container)
        self.canvas.pack(side="left", fill="both", expand=True)

        scrollbar = tk.Scrollbar(container, orient="vertical", command=self.canvas.yview)
        scrollbar.pack(side="right", fill="y")

        self.canvas.configure(yscrollcommand=scrollbar.set)

        # This frame goes inside the canvas
        self.scrollable_frame = tk.Frame(self.canvas)
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all"))
        )
        self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        # ----------------------------------------------------------------------

        # For storing data and references
        self.voltage_frames = []  # [module][row][col] -> the Frame containing sparkline
        self.temp_frames = []  # [module][row][col] -> the Frame containing sparkline
        self.voltage_canvases = []
        self.temp_canvases = []

        # Historical data storage for each sensor (up to 60 readings)
        self.voltage_history = [[[[] for _ in range(5)] for _ in range(4)] for _ in range(5)]
        self.temp_history = [[[[] for _ in range(6)] for _ in range(3)] for _ in range(5)]

        # Dictionary for flashing frames
        self.flashing_frames = {}

        # Keep references to each module frame so we can update the title
        self.module_frames = []

        # Loop over 5 modules
        for module in range(5):
            # LabelFrame for the module (title includes total voltage and average temperature)
            module_frame = tk.LabelFrame(
                self.scrollable_frame,
                text=f"Module {module + 1} - Total Voltage: 0.000 - Avg Temp: 0.0°C",
                padx=5, pady=5
            )
            module_frame.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
            self.module_frames.append(module_frame)

            # Arrays for the frames/canvases in this module
            mod_volt_frames = []
            mod_volt_canvases = []
            mod_temp_frames = []
            mod_temp_canvases = []

            # Voltage grid: 5 columns x 4 rows (20 cells)
            voltage_frame = tk.LabelFrame(module_frame, text="Voltage (5x4 grid)", padx=5, pady=5)
            voltage_frame.grid(row=0, column=0, padx=5, pady=5)
            for row in range(4):
                row_frames = []
                row_canvases = []
                for col in range(5):
                    # Each cell: a Frame with background color plus a Canvas for the sparkline
                    cell_frame = tk.Frame(voltage_frame, width=80, height=30, bg="white", relief=tk.RIDGE,
                                          borderwidth=2)
                    cell_frame.grid_propagate(False)  # keep fixed size
                    cell_frame.grid(row=row, column=col, padx=2, pady=2)

                    # Create a Canvas for sparkline
                    c = tk.Canvas(cell_frame, width=80, height=30, bg="black", highlightthickness=0)
                    c.pack(fill="both", expand=True)

                    # Attach sensor info for updating raw value
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

            # Temperature grid: 6 columns x 3 rows (18 sensors)
            temp_frame = tk.LabelFrame(module_frame, text="Temperature (6x3 grid)", padx=5, pady=5)
            temp_frame.grid(row=0, column=1, padx=5, pady=5)
            for row in range(3):
                row_frames = []
                row_canvases = []
                for col in range(6):
                    cell_frame = tk.Frame(temp_frame, width=80, height=30, bg="white", relief=tk.RIDGE, borderwidth=2)
                    cell_frame.grid_propagate(False)
                    cell_frame.grid(row=row, column=col, padx=2, pady=2)

                    c = tk.Canvas(cell_frame, width=80, height=30, bg="black", highlightthickness=0)
                    c.pack(fill="both", expand=True)

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

        # Begin periodic updates
        self.update_values()

    def flash_toggle(self, frame):
        """
        Toggles the flashing state of a frame by alternating between the base color
        and FLASH_COLOR.
        """
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
        """
        Begins flashing for the given frame if not already active.
        """
        if frame in self.flashing_frames:
            self.flashing_frames[frame]["base_color"] = base_color
            return
        self.flashing_frames[frame] = {"state": True, "base_color": base_color, "job": None}
        self.flash_toggle(frame)

    def stop_flashing(self, frame, base_color):
        """
        Stops flashing on the given frame and resets its background to the base color.
        """
        if frame in self.flashing_frames:
            job = self.flashing_frames[frame].get("job")
            if job:
                self.root.after_cancel(job)
            frame.config(bg=base_color)
            del self.flashing_frames[frame]

    def update_values(self):
        """
        Simulates new sensor readings, updates the sparkline and raw value display,
        color-codes (and flashes) cells, computes module-level total voltage and average temperature,
        and updates the global total voltage.
        """
        global_total_voltage = 0.0

        for module in range(5):
            module_voltage_sum = 0.0
            module_temp_sum = 0.0
            voltage_count = 0
            temp_count = 0

            # Update each voltage cell
            for row in range(4):
                for col in range(5):
                    voltage = round(random.uniform(2.5, 4.0), 3)
                    module_voltage_sum += voltage
                    voltage_count += 1

                    # Append to historical data (limit to 60 readings)
                    hist = self.voltage_history[module][row][col]
                    hist.append(voltage)
                    if len(hist) > 60:
                        hist.pop(0)

                    # Determine color and flashing based on thresholds
                    base_color = coolwarm_color(voltage, VOLTAGE_LOW_THRESHOLD, VOLTAGE_HIGH_THRESHOLD)
                    frame = self.voltage_frames[module][row][col]
                    if voltage < VOLTAGE_LOW_THRESHOLD or voltage > VOLTAGE_HIGH_THRESHOLD:
                        self.start_flashing(frame, base_color)
                    else:
                        self.stop_flashing(frame, base_color)
                        frame.config(bg=base_color)

                    # Draw sparkline with raw value on the canvas
                    canvas = self.voltage_canvases[module][row][col]
                    draw_sparkline(canvas, hist, VOLTAGE_LOW_THRESHOLD, VOLTAGE_HIGH_THRESHOLD)

            # Update each temperature cell
            for row in range(3):
                for col in range(6):
                    temp = random.randint(5, 45)
                    module_temp_sum += temp
                    temp_count += 1

                    hist = self.temp_history[module][row][col]
                    hist.append(temp)
                    if len(hist) > 60:
                        hist.pop(0)

                    base_color = coolwarm_color(temp, TEMP_LOW_THRESHOLD, TEMP_HIGH_THRESHOLD)
                    frame = self.temp_frames[module][row][col]
                    if temp < TEMP_LOW_THRESHOLD or temp > TEMP_HIGH_THRESHOLD:
                        self.start_flashing(frame, base_color)
                    else:
                        self.stop_flashing(frame, base_color)
                        frame.config(bg=base_color)

                    canvas = self.temp_canvases[module][row][col]
                    draw_sparkline(canvas, hist, TEMP_LOW_THRESHOLD, TEMP_HIGH_THRESHOLD)

            # Update module title with computed totals
            avg_temp = module_temp_sum / temp_count if temp_count else 0
            self.module_frames[module].config(
                text=(
                    f"Module {module + 1} - "
                    f"Total Voltage: {module_voltage_sum:.3f} - "
                    f"Avg Temp: {avg_temp:.1f}°C"
                )
            )
            global_total_voltage += module_voltage_sum

        self.global_total_voltage_label.config(text=f"Global Total Voltage: {global_total_voltage:.3f}")

        # Schedule next update in 1 second
        self.root.after(1000, self.update_values)


if __name__ == "__main__":
    root = tk.Tk()
    app = BMSSimulatorGUI(root)
    root.mainloop()