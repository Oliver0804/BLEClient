# Arduino Based Bluetooth Data Parsing

This project is designed to interface with Bluetooth devices using the Realtek RTL8710 (BW16) module under the Arduino 1.8.9 environment. It enables the Arduino to connect to Bluetooth devices and receive data from them, including battery level, button state, and IMU (Inertial Measurement Unit) readings.

## Features

- Scan for and connect to Bluetooth devices.
- Parse data from connected Bluetooth devices.
- Read battery service data.
- Read button state from the connected device.
- Configure and read IMU data (accelerometer and gyroscope).

## Prerequisites

Before you begin, ensure you have met the following requirements:

- Arduino IDE 1.8.9 or later installed.
- Realtek RTL8710 (BW16) module.
- A Bluetooth device broadcasting the required services (battery level, button state, IMU data).

## Installation

1. Clone this repository to your local machine or download the ZIP file.
2. Open the project in the Arduino IDE.
3. Connect the Realtek RTL8710 module to your computer.
4. Select the appropriate board and port in the Arduino IDE.
5. Upload the code to the module.

## Usage

After uploading the code to the RTL8710 module, it will automatically start scanning for Bluetooth devices. The device specifically looks for a device named `Lapita_3280`. Once connected, it will begin reading and displaying data from the device's services:

- Battery Level
- Button State
- IMU Data (6-axis accelerometer and gyroscope)

You can modify the code to connect to other Bluetooth devices or to handle different data types based on your project requirements.

## Contributing

Contributions to this project are welcome. Please fork the repository and submit a pull request with your changes.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

Thanks to the Realtek team for providing the RTL8710 SDK and to the Arduino community for the comprehensive libraries and examples.

