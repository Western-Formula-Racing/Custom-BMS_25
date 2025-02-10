import tkinter as tk
from tkinter import ttk

# Creating root directory
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import sv_ttk
import darkdetect
from GUI.components.Module_Battery.Modules_Holder import ModulesHolder  # Import ModulesHolder
from GUI.components.Module_Battery.Modules.Module_Information import ModuleInformation
from GUI.components.Module_Battery.Modules.Module_CoreINFO import CoreInfo

class BMS_GUI_START_PAGE:
    def __init__(self, root):
        self.root = root
        self.module_settings_frame = None
        self.setup_gui()

    def setup_gui(self):
        self.root.title("BMS GUI")
        self.root.geometry("1200x600")

        # Create the main notebook (tabbed interface)
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill="both", expand=True)

        # Battery Profile Tab
        self.battery_profile_tab = ttk.Frame(self.notebook)
        self.notebook.add(self.battery_profile_tab, text="Battery Profile")

        # Create a frame for the content
        self.content_frame = ttk.Frame(self.battery_profile_tab)
        self.content_frame.pack(fill="both", expand=True)

        # Modules Holder (Top Row, Full Width)
        self.modules_holder = ModulesHolder(self.content_frame, self.update_module_content)
        self.modules_holder.grid(row=0, column=0, columnspan=2, sticky="ew", padx=5, pady=5)

        # Core Info (Left Column)
        self.core_info_frame = CoreInfo(self.content_frame, module_id=1)  # Default to Module 1
        self.core_info_frame.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)

        # Module Information (Right Column)
        self.module_settings_frame = ModuleInformation(self.content_frame, module_id=1, close_tab_callback=self.clear_module_settings)
        self.module_settings_frame.grid(row=1, column=1, sticky="nsew", padx=5, pady=5)

        # Configure layout
        self.content_frame.grid_rowconfigure(1, weight=1)
        self.content_frame.grid_columnconfigure(0, weight=1)  # Core Info adjusts width dynamically
        self.content_frame.grid_columnconfigure(1, weight=1)  # Module Information adjusts width dynamically

    def update_module_content(self, module_id):
        # Update Core Info
        self.core_info_frame.destroy()
        self.core_info_frame = CoreInfo(self.content_frame, module_id)
        self.core_info_frame.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)

        # Update Module Information
        if self.module_settings_frame is not None:
            self.module_settings_frame.destroy()  # Safely destroy the previous frame
        self.module_settings_frame = ModuleInformation(self.content_frame, module_id, close_tab_callback=self.clear_module_settings)
        self.module_settings_frame.grid(row=1, column=1, sticky="nsew", padx=5, pady=5)

        # Reset column weights
        self.content_frame.grid_columnconfigure(0, weight=1)
        self.content_frame.grid_columnconfigure(1, weight=1)

    def clear_module_settings(self):
        # Safely destroy the module settings frame
        if self.module_settings_frame is not None:
            self.module_settings_frame.destroy()
            self.module_settings_frame = None  # Reset to None

        # Adjust layout to give full width to Core Info
        self.content_frame.grid_columnconfigure(0, weight=1)
        self.content_frame.grid_columnconfigure(1, weight=0)  # Remove weight from the right column


if __name__ == "__main__":
    root = tk.Tk()
    sv_ttk.set_theme("light")
    gui = BMS_GUI_START_PAGE(root)
    root.mainloop()