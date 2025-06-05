בהתבסס על מבנה הקבצים בפרויקט שלך, הנה קובץ README.md באנגלית, מוכן להעתקה:

# PingAndWatchdogTimer

This project demonstrates process management and inter-process communication in C, focusing on implementing a custom ping utility and a watchdog timer mechanism.

## Features

- **Custom Ping Utility (`myping.c`)**: Sends ICMP echo requests to a specified host and measures the response time, similar to the standard `ping` command.

- **Watchdog Timer**: Monitors a child process and restarts it if it becomes unresponsive or terminates unexpectedly.

## Project Structure

PingAndWatchdogTimer/

├── myping.c                  # Custom ping implementation

├── fork + exec/              # Contains watchdog timer implementation

├── parta.pcapng              # Packet capture file for part A

├── partb-with-sleep.pcapng   # Packet capture with sleep in part B

├── partb-without-sleep.pcapng# Packet capture without sleep in part B

├── partb-without-wifi.pcapng # Packet capture without Wi-Fi in part B

├── .vscode/                  # VSCode configuration files

├── __MACOSX/                 # macOS metadata (can be ignored)

## Compilation

To compile the `myping` program:

```bash
gcc -o myping myping.c
```

To compile the watchdog timer program (assuming it’s named watchdog.c inside the fork + exec directory):

```bash
gcc -o watchdog "fork + exec"/watchdog.c
```

## Usage

### Custom Ping Utility

```bash
./myping <hostname>
```

Replace <hostname> with the target host’s domain name or IP address.

### Watchdog Timer

```bash
./watchdog
```

This will start the watchdog process, which in turn starts and monitors the child process.

## Packet Captures

The .pcapng files are packet capture files that can be opened with tools like Wireshark for analysis:
	•	parta.pcapng: Capture from part A.
	•	partb-with-sleep.pcapng: Capture from part B with sleep intervals.
	•	partb-without-sleep.pcapng: Capture from part B without sleep intervals.
	•	partb-without-wifi.pcapng: Capture from part B without Wi-Fi.

## Project by

Avichai Mizrachi
