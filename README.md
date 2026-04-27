# The-Auto-Cooler-Closed-Loop-System
Closed-loop STM32F4 system using a Mealy State Machine to control a DC fan via PWM. Reads LM35 sensor data (ADC+DMA) and displays live Temp/Fan speed on a parallel LCD1602. Features IDLE, COOLING, and OVERHEAT states with an alarm LED for safety. Developed with modular, non-polling drivers.
