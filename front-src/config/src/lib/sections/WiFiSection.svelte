<script lang="ts">
  import { resolve } from "$app/paths";
  import type { ConfigData, WifiNetwork } from "$lib/types";
  import { INPUT_CLASS } from "$lib/styles";

  let { config = $bindable() } = $props<{ config: ConfigData }>();

  let isManualWifi = $state(false);

  // NEW: Store the networks in a real array
  let scannedNetworks = $state<WifiNetwork[]>([]);
  let isScanning = $state(false);
  const isDev = import.meta.env.DEV;

  // NEW: Check if the current saved SSID exists in the scan results
  // This 'derived' logic happens automatically in Svelte 5
  let isSavedNetworkInList = $derived(
    scannedNetworks.some((n) => n.ssid === config.wifiSsid),
  );

function triggerScan() {
    isScanning = true;
    const endpoint = isDev ? resolve('/api/scan.json' as any) : '/api/scan';
    
    fetch(endpoint)
      .then(r => r.json())
      .then((data: WifiNetwork[]) => {
        // --- Deduplication Logic (Mesh Fix) ---
        const uniqueMap = new Map<string, WifiNetwork>();

        data.forEach(net => {
          // If we haven't seen this SSID yet...
          // OR if this new signal is stronger than the one we have...
          const existing = uniqueMap.get(net.ssid);
          if (!existing || net.rssi > existing.rssi) {
             uniqueMap.set(net.ssid, net);
          }
        });

        // Convert Map back to Array and sort
        scannedNetworks = Array.from(uniqueMap.values())
            .sort((a, b) => b.rssi - a.rssi);
      })
      .catch(err => {
        console.error("Scan failed", err);
        scannedNetworks = [];
      })
      .finally(() => isScanning = false);
      
  }

  function handleNetworkSelect(event: Event) {
    const select = event.target as HTMLSelectElement;
    if (select.value === "_MANUAL_") {
      isManualWifi = true;
      config.wifiSsid = "";
    }
  }

  function getSignalBars(rssi: number): string {
    if (rssi >= -55) return '●●●'; // Excellent
    if (rssi >= -67) return '●●-'; // Good
    if (rssi >= -80) return '●--'; // Fair
    return '---';                  // Weak
  }
  
</script>

<section class="space-y-3">
  <h2 class="text-lg font-semibold text-gray-700 border-b pb-1">
    🏠 Local Wi-Fi
  </h2>
  <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
    <label class="block">
      <span class="text-gray-700 text-sm font-bold">Wi-Fi Network</span>
      <div class="flex gap-2 mt-1">
        {#if isManualWifi}
          <!-- We need the focus to jump to the component when selecting "Enter Manually"-->
          <!-- svelte-ignore a11y_autofocus -->
          <input
            type="text"
            bind:value={config.wifiSsid}
            class={`${INPUT_CLASS} animate-fade-in`}
            placeholder="Type SSID..."
            autoFocus
          />
          <button
            onclick={() => (isManualWifi = false)}
            class="bg-red-100 text-red-700 px-3 py-2 rounded-md">✕</button
          >
        {:else}
          <select
            bind:value={config.wifiSsid}
            onchange={handleNetworkSelect}
            class="{INPUT_CLASS} bg-white"
            disabled={isScanning}
          >
            <option value="" disabled>Select a network...</option>

            {#if config.wifiSsid && !isSavedNetworkInList}
              <option value={config.wifiSsid}>
                {config.wifiSsid} (Saved - Not in range)
              </option>
            {/if}

            {#if isScanning}
              <option disabled>Scanning...</option>
            {:else}
              {#each scannedNetworks as net}
                <option value={net.ssid}>
                  {net.ssid} ({getSignalBars(net.rssi)} {net.auth === 0 ? "🔓" : "🔒"})
                </option>
              {/each}
            {/if}

            <option value="_MANUAL_" class="font-bold text-blue-600 bg-blue-50">
              ➕ Enter Manually...
            </option>
          </select>

          <button
            onclick={triggerScan}
            class="bg-gray-200 hover:bg-gray-300 px-3 py-2 rounded-md"
          >
            {isScanning ? "⏳" : "🔍"}
          </button>
        {/if}
      </div>
    </label>

    <label class="block">
      <span class="text-gray-700 text-sm font-bold">Wi-Fi Password</span>
      <input
        type="password"
        bind:value={config.wifiPassword}
        class="mt-1 block w-full rounded-md border-gray-300 shadow-sm border p-2"
      />
    </label>
  </div>
</section>
