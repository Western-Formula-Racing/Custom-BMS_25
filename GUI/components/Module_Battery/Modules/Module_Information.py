import tkinter as tk
from tkinter import ttk
import importlib
import sys
import os

sys.path.append(os.path.abspath(os.path.dirname(__file__)))

from GUI.components.Module_Battery.Modules.ModuleN_Battery import get_module_data 


class ModuleInformation(tk.Frame):
    def __init__(self, parent, module_id, close_tab_callback):
        super().__init__(parent)
        self.module_id = module_id
        self.close_tab_callback = close_tab_callback
        self.module_data = self.load_module_data()
        self.create_content()

    def load_module_data(self):
        # Dynamically load data for the given module_id
        try:
            return get_module_data(self.module_id)  # Pass the module ID to get data
        except Exception as e:
            return {"Error": f"An unexpected error occurred: {str(e)}"}

    def create_content(self):
        # Title label
        title_label = tk.Label(self, text=f"Information For Module {self.module_id}", font=("Arial", 18, "bold"))
        title_label.grid(row=0, column=0, columnspan=5, sticky="ew", pady=10)

        # Settings buttons
        self.create_module_settings()

        # Display other module data
        row_index = 2  # Start placing content after the settings row
        for key, value in self.module_data.items():
            if key == "Cell Voltages":
                # Create a grid for cell voltages
                self.create_table(value, "Cell Voltages", row_index, columns=4)
                row_index += len(value) + 1
            elif key == "Temperature Sensor":
                # Create a table for temperature sensors
                self.create_table(value, "Temperature Sensors", row_index, columns=6)
                row_index += (len(value) // 6) + 2
            elif key == "Balancing Current":
                # Create a table for balancing current
                self.create_table(value, "Balancing Current", row_index, columns=5)
                row_index += (len(value) // 5) + 2
            else:
                # Display other key-value pairs
                data_label = tk.Label(self, text=f"{key}: {value}", font=("Arial", 12))
                data_label.grid(row=row_index, column=0, columnspan=5, sticky="w", padx=10, pady=2)
                row_index += 1

        # Add a close button
        close_button = tk.Button(self, text="Close Tab", command=self.close_tab_callback)
        close_button.grid(row=row_index, column=0, columnspan=5, pady=10)

    def create_module_settings(self):
        # Buttons for settings (e.g., Temp, Discharge, Charge, SOC)
        settings_frame = tk.Frame(self)  # Create a frame to hold the buttons
        settings_frame.grid(row=1, column=0, columnspan=5, sticky="ew", padx=5, pady=5)

        # Configure the grid column weights for equal spacing
        self.columnconfigure(0, weight=1)  # Ensure the parent frame stretches horizontally
        settings_frame.columnconfigure(0, weight=1)
        settings_frame.columnconfigure(1, weight=1)
        settings_frame.columnconfigure(2, weight=1)
        settings_frame.columnconfigure(3, weight=1)
        settings_frame.columnconfigure(4, weight=1)

        # Define settings and create buttons
        settings = ["Temp", "Discharge", "Charge", "SOC", "Cell Settings"]
        for index, setting in enumerate(settings):
            setting_button = tk.Button(
                settings_frame,
                text=setting,
                command=lambda s=setting: self.handle_setting_click(s)
            )
            setting_button.grid(row=0, column=index, sticky="ew", padx=5, pady=5)  # Use grid layout for equal spacing

    def create_table(self, data, title, start_row, columns):
        """Create a grid-style table to display a list of data."""
        grid_frame = tk.Frame(self)
        grid_frame.grid(row=start_row, column=0, columnspan=5, pady=10, sticky="ew")

        # Title
        title_label = tk.Label(grid_frame, text=title, font=("Arial", 14, "bold"))
        title_label.grid(row=0, column=0, columnspan=columns, pady=5)

        # Create the table
        for i, value in enumerate(data):
            row, col = divmod(i, columns)
            cell_label = tk.Label(grid_frame, text=value, borderwidth=1, relief="solid", width=15)
            cell_label.grid(row=row + 1, column=col, padx=2, pady=2)

    def handle_setting_click(self, setting):
        # Handle actions for each setting button
        print(f"{setting} button clicked for Module {self.module_id}")
        # Here, you can add logic to open specific setting details


if __name__ == "__main__":
    root = tk.Tk()
    root.geometry("800x600")
    app = ModuleInformation(root, module_id=1, close_tab_callback=lambda: print("Tab Closed"))
    app.pack(fill="both", expand=True)
    root.mainloop()