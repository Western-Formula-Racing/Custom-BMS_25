import tkinter as tk

class ModulesHolder(tk.Frame):
    def __init__(self, parent, switch_to_content_callback):
        super().__init__(parent)
        self.switch_to_content_callback = switch_to_content_callback
        self.create_modules_grid()

    def create_modules_grid(self):
        # Add a title for the modules
        title_label = tk.Label(self, text="Modules Connected", font=("Arial", 24))
        title_label.grid(row=0, column=0, columnspan=5, pady=5, sticky="ew")  # Span the title across all columns

        # Create buttons for each module in a horizontal layout
        for i in range(1, 6):  # Example: 5 modules
            module_button = tk.Button(
                self,
                text=f"Module {i}",
                command=lambda module_id=i: self.switch_to_content_callback(module_id)
            )
            module_button.grid(row=1, column=i-1, padx=5, pady=10, sticky="ew")  # Position buttons with even spacing

        # Make all columns expand equally
        for col in range(5):
            self.grid_columnconfigure(col, weight=1)

if __name__ == "__main__":
    root = tk.Tk()
    root.geometry("800x200")
    root.title("Modules Holder")

    def dummy_callback(module_id):
        print(f"Module {module_id} selected.")

    app = ModulesHolder(root, switch_to_content_callback=dummy_callback)
    app.pack(expand=1, fill="both")
    root.mainloop()
