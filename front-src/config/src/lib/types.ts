  // Interface definition
  export interface ConfigData {
    apSsid: string;
    apPassword?: string;
    wifiSsid: string;
    wifiPassword?: string;
    weatherApiKey: string;
    weatherCity: string;
    ntpServer: string;
    timeZone: string;
  }

  // Interface for the Scan Result
  export interface WifiNetwork {
    ssid: string;
    rssi: number;
    auth: number;
  }