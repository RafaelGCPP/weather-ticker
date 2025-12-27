<script lang="ts">
  import { onMount } from "svelte";
  import { resolve } from '$app/paths';

  // Interface definition
  interface ConfigData {
    apSsid: string;
    apPassword?: string;
    wifiSsid: string;
    wifiPassword?: string;
    weatherApiKey: string;
    weatherCity: string;
    ntpServer: string;
    timeZone: string;
  }

  // --- STATE (Runes) ---
  let config: ConfigData = $state({
    apSsid: "",
    apPassword: "",
    wifiSsid: "",
    wifiPassword: "",
    weatherApiKey: "",
    weatherCity: "",
    ntpServer: "",
    timeZone: "",
  });

  // Interface for the Scan Result
  interface WifiNetwork {
    ssid: string;
    rssi: number;
    auth: number;
  }

  let loadPromise = $state<Promise<any> | null>(null);
  let scanPromise = $state<Promise<WifiNetwork[]> | null>(null);
  let savePromise = $state<Promise<string> | null>(null);

  // --- ENVIRONMENT CHECK ---
  // If we are on PC (Dev), use .json files. If on ESP32 (Prod), use API endpoints.
  const isDev = import.meta.env.DEV;

  // --- ACTIONS ---
  onMount(() => {
    const endpoint = isDev ? resolve("/api/config.json" as any) : "/api/config";

    loadPromise = fetch(endpoint)
      .then((r) => {
        if (!r.ok) throw new Error("Error loading configuration.");
        return r.json();
      })
      .then((data) => {
        Object.assign(config, data);
        return data;
      });
  });

  function triggerScan() {
    const endpoint = isDev ? resolve("/api/scan.json" as any) : "/api/scan";

    // Scan might take time on ESP32, so we just fetch the list
    scanPromise = fetch(endpoint)
      .then((r) => r.json())
      .then((data: WifiNetwork[]) => data.sort((a, b) => b.rssi - a.rssi));
  }

  function saveAll() {
    // MOCK SAVE (For PC Development)
    if (isDev) {
      savePromise = new Promise((resolve) => {
        console.log("Mock Save Payload:", JSON.stringify(config, null, 2));
        setTimeout(() => resolve("Saved successfully (Mock)!"), 1500);
      });
      return;
    }

    // REAL SAVE (For ESP32)
    savePromise = fetch("/api/save", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(config),
    }).then(async (r) => {
      if (!r.ok) throw new Error("Error saving to Flash");
      return "Configuration saved! Rebooting...";
    });
  }
</script>

<div
  class="w-full max-w-2xl bg-white shadow-lg rounded-xl overflow-hidden border border-gray-200"
>
  <div class="bg-blue-600 p-4">
    <h1 class="text-white text-xl font-bold text-center">
      System Configuration
    </h1>
  </div>

  <div class="p-6 space-y-6">
    {#if loadPromise}
      {#await loadPromise}
        <div class="text-center text-gray-500 py-10 animate-pulse">
          Loading configuration...
        </div>
      {:then _}
        <section class="space-y-3">
          <h2 class="text-lg font-semibold text-gray-700 border-b pb-1">
            📡 SoftAP Settings
          </h2>
          <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
            <label class="block">
              <span class="text-gray-700 text-sm font-bold">AP SSID</span>
              <input
                type="text"
                bind:value={config.apSsid}
                class="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-blue-500 focus:ring-blue-500 border p-2"
              />
            </label>
            <label class="block">
              <span class="text-gray-700 text-sm font-bold">AP Password</span>
              <input
                type="password"
                bind:value={config.apPassword}
                placeholder="Optional (Leave empty to keep)"
                class="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-blue-500 focus:ring-blue-500 border p-2"
              />
            </label>
          </div>
        </section>

        <section class="space-y-3">
          <h2 class="text-lg font-semibold text-gray-700 border-b pb-1">
            🏠 Local Wi-Fi
          </h2>
          <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
            <label class="block">
              <span class="text-gray-700 text-sm font-bold">Wi-Fi Network</span>
              <div class="flex gap-2 mt-1">
                <input
                  type="text"
                  list="wifi-list"
                  bind:value={config.wifiSsid}
                  class="block w-full rounded-md border-gray-300 shadow-sm focus:border-blue-500 focus:ring-blue-500 border p-2"
                  placeholder="Network name"
                />
                <button
                  onclick={triggerScan}
                  class="bg-gray-200 hover:bg-gray-300 text-gray-700 px-3 py-2 rounded-md transition"
                  >🔍</button
                >
              </div>

              <datalist id="wifi-list">
                {#if scanPromise}
                  {#await scanPromise then networks}
                    {#each networks as net}
                      <option value={net.ssid}>
                        {net.auth === 0 ? "🔓" : "🔒"}
                        {net.rssi} dBm
                      </option>
                    {/each}
                  {/await}
                {/if}
              </datalist>
            </label>

            <label class="block">
              <span class="text-gray-700 text-sm font-bold">Wi-Fi Password</span
              >
              <input
                type="password"
                bind:value={config.wifiPassword}
                class="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-blue-500 focus:ring-blue-500 border p-2"
              />
            </label>
          </div>
          {#if scanPromise}
            {#await scanPromise}
              <p class="text-xs text-blue-500">Scanning for networks...</p>
            {/await}
          {/if}
        </section>

        <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
          <section class="space-y-3">
            <h2 class="text-lg font-semibold text-gray-700 border-b pb-1">
              ⛅ Weather Service
            </h2>
            <label class="block">
              <span class="text-gray-700 text-sm font-bold"
                >OpenWeather Key</span
              >
              <input
                type="text"
                bind:value={config.weatherApiKey}
                class="mt-1 block w-full rounded-md border-gray-300 shadow-sm font-mono text-sm border p-2"
              />
            </label>
            <label class="block">
              <span class="text-gray-700 text-sm font-bold">City, Country</span>
              <input
                type="text"
                bind:value={config.weatherCity}
                class="mt-1 block w-full rounded-md border-gray-300 shadow-sm border p-2"
              />
            </label>
          </section>

          <section class="space-y-3">
            <h2 class="text-lg font-semibold text-gray-700 border-b pb-1">
              🕒 Time (NTP)
            </h2>
            <label class="block">
              <span class="text-gray-700 text-sm font-bold">NTP Server</span>
              <input
                type="text"
                bind:value={config.ntpServer}
                class="mt-1 block w-full rounded-md border-gray-300 shadow-sm border p-2"
              />
            </label>
            <label class="block">
              <span class="text-gray-700 text-sm font-bold"
                >Timezone String</span
              >
              <input
                type="text"
                bind:value={config.timeZone}
                class="mt-1 block w-full rounded-md border-gray-300 shadow-sm border p-2"
              />
            </label>
          </section>
        </div>

        <div class="pt-4">
          <button
            onclick={saveAll}
            class="w-full bg-blue-600 hover:bg-blue-700 text-white font-bold py-3 px-4 rounded shadow transition transform active:scale-95"
          >
            💾 Save Configuration
          </button>
        </div>

        {#if savePromise}
          {#await savePromise}
            <div class="mt-4 p-3 bg-blue-100 text-blue-800 rounded text-center">
              Saving...
            </div>
          {:then msg}
            <div
              class="mt-4 p-3 bg-green-100 text-green-800 rounded text-center"
            >
              {msg}
            </div>
          {:catch}
            <div class="mt-4 p-3 bg-red-100 text-red-800 rounded text-center">
              Error saving configuration.
            </div>
          {/await}
        {/if}
      {:catch error}
        <div class="text-red-500 text-center py-10">
          <p class="font-bold">Connection Error</p>
          <p class="text-sm">{error.message}</p>
          <p class="text-xs text-gray-500 mt-2">
            (Check if Mock JSON exists in static/api/)
          </p>
        </div>
      {/await}
    {/if}
  </div>
</div>
