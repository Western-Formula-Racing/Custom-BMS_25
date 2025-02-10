def get_module_data(module_id):
    # Validate module_id
    if module_id < 1 or module_id > 5:  # Assuming there are 5 modules (1 to 5)
        raise ValueError(f"Invalid module_id: {module_id}. Must be between 1 and 5.")

    # Generate data based on module ID
    return {
        "Cell Voltages": [f"Vcell {((module_id - 1) * 20) + i + 1}" for i in range(20)],
        "Temperature Sensor": [f"Thermistor {((module_id - 1) * 18) + i + 1}" for i in range(18)],
        "Balancing Current": [f"I{((module_id - 1) * 20) + i + 1}" for i in range(20)]
    }
