# Weather Ticker Configuration Example

## Serial Console Output Example

When the device starts in SoftAP mode, you'll see output like this:

```
I (1234) wifi_manager: Initializing Wi-Fi Manager
I (1235) wifi_manager: No saved Wi-Fi credentials found, starting SoftAP
I (1240) wifi_manager: 
I (1241) wifi_manager: ===========================================
I (1242) wifi_manager:   SoftAP Mode Active
I (1243) wifi_manager: ===========================================
I (1244) wifi_manager:   SSID: Weather
I (1245) wifi_manager:   PSK:  A7B2C9D4E5F6
I (1246) wifi_manager: ===========================================
I (1247) wifi_manager: 
I (1248) wifi_manager: SoftAP started successfully
I (1249) main: System initialized successfully
```

## Connecting to the SoftAP

1. Look for a Wi-Fi network named "Weather" (or your custom SSID)
2. Connect using the password shown in the serial console
3. Once connected, you'll be able to access the configuration interface (future feature)

## Notes

- The SoftAP password is randomly generated on first boot and saved to NVS
- The password will remain the same across reboots unless you erase the NVS partition
- To reset the SoftAP credentials, erase the NVS partition using: `idf.py erase-flash`
