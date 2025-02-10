import tkinter as tk

class CoreInfo(tk.Frame):
    def __init__(self, parent, module_id):
        super().__init__(parent)
        self.module_id = module_id
        self.extra_info_frame = None  # Frame to hold extra information
        self.show_extra_info = False  # Toggle state for extra info visibility
        self.create_content()

    def create_content(self):
        # Example core information
        core_data = {
            "Module Voltage": "3.7V",
            "Module SOC": "85%",
            "Average Temperature": "25°C",
            "Dissipated Charge": "4000mAh",
            "Voltage Upper Limit": "3.6V",
            "Voltage Lower Limit": "1.7V"
        }

        extra_info = {
            "LTC6813 Side A Temperature": "Tltc A",
            "LTC6813 Side B Temperature": "Tltc B"
        }

        # Display core information
        title_label = tk.Label(self, text=f"Core Info for Module {self.module_id}", font=("Arial", 18, "bold"))
        title_label.pack(pady=15)

        core_info_label = tk.Label(self, text="Core Information:", font=("Arial", 14), anchor="w")
        core_info_label.pack(fill="x", padx=10, pady=5)

        for key, value in core_data.items():
            data_label = tk.Label(self, text=f"{key}: {value}", font=("Arial", 12), anchor="w")
            data_label.pack(fill="x", padx=20, pady=2)

        # Button to toggle extra information
        toggle_button = tk.Button(self, text="Show Extra Info", font=("Arial", 14), command=self.toggle_extra_info)
        toggle_button.pack(pady=10)

        # Frame for extra information (initially hidden)
        self.extra_info_frame = tk.Frame(self)
        self.extra_info_frame.pack(fill="x", padx=10, pady=5)

        # Add extra information header and labels to the frame
        extra_info_label = tk.Label(self.extra_info_frame, text="Extra Information:", font=("Arial", 14), anchor="w")
        extra_info_label.pack(fill="x", padx=10, pady=5)

        for key, value in extra_info.items():
            data_label = tk.Label(self.extra_info_frame, text=f"{key}: {value}", font=("Arial", 12), anchor="w")
            data_label.pack(fill="x", padx=20, pady=2)

        # Hide extra info by default
        self.extra_info_frame.pack_forget()

    def toggle_extra_info(self):
        # Toggle the visibility of the extra information frame
        if self.show_extra_info:
            self.extra_info_frame.pack_forget()
            self.show_extra_info = False
        else:
            self.extra_info_frame.pack(fill="x", padx=10, pady=5)
            self.show_extra_info = True
