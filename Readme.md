# IR Remote to controls the Electra Minimax AC and its dampers

The ESP microcontroller is responsible for controlling the Electra Minimax AC and its dampers.  
Each ESP device is uniquely identified by its BLE MAC address, which allows the system to precisely locate and control individual devices in a multi-device setup.  
This ensures that commands are sent to the correct ESP device, enabling seamless operation across multiple units.  

Each ESP functions as a BLE peripheral, meaning it advertises its presence and waits for commands from a central controller.  
The central controller, referred to as the BLE master, acts as the remote control for all connected ESP peripherals.  
The BLE master sends commands to the ESP devices, which then translate these commands into IR signals to control the AC and its dampers.
This architecture allows for centralized control of multiple ESP devices, providing a scalable and efficient solution for managing multiple AC units and dampers.

## Developer

Developed by Tamir Mhabary

## Features

- BLE communication to control IR devices.
- Easy integration with mobile or desktop applications.
- Support for multiple IR protocols.

## Folder Structure

```
The project is organized as follows:
BlePerepherial/
├── src/                
│   └── main.cpp         # Main application code
├── lib/                
│   ├── BLEServerManager/
│   │   ├── BLEServerManager.cpp # BLE server management logic
│   │   └── BLEServerManager.h   # BLE server management header             
│   ├── deep_sleep/
│   │   └── deep_sleep.h         # Deep sleep functionality
│   ├── global_vars/
│   │   └── global_var.h         # Global variables
│   └── IR/                      
│       └── ir.h                 # IR-related functionality
├── 3d/                
│   ├── case_bottom.STL          # STL file for the bottom part of the case
│   ├── case_up.STL              # STL file for the upper part of the case
│   ├── Case_w_out_top.png       # Image of the case without the top
│   └── Case.png                 # Image of the complete case
└── Readme.md                    # Project documentation
```

## 3D Printed Parts

The project includes 3D printable STL files for the custom case required for the setup.  
These files, along with their images, are located in the `3d` folder.

### Preview of 3D Parts

 **Case**  

  View: 3D isometric view  
  ![Case Top](3d/Case.png)

  View: Board without top part  
  ![Case Bottom](3d/Case_w_out_top.png)

### Prerequisites

- ESP32-C3 Super Mini microcontroller.  
- IR transmitter and receiver modules
- Development environment set up for your ESP32-C3 (e.g., Arduino IDE, PlatformIO).

### Installation

1. Clone this repository:
```
   git clone https://github.com/tamirmha/AC-Control.git
```
2. Open the project in your development environment.
3. Flash the firmware to your ESP32-C3 Super Mini.

### Usage

1. Power on the BLE peripheral device.
2. Connect to the device using a BLE-compatible application.
3. Send IR commands via the BLE interface.

### Electronic Schematic

Below is the electronic schematic for the project:

![Electronic Schematic](Schematic_IR_2025-03-31.png)

## Contributions

Contributions are welcome! Please fork the repository and submit a pull request.

## License

This project is licensed under the MIT License. See the LICENSE file for details.
