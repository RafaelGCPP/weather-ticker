<script lang="ts">
  import { resolve } from '$app/paths';
  import type { ConfigData, WifiNetwork } from '$lib/types'; // Import shared types
  import { INPUT_CLASS } from '$lib/styles';

  // Receive the main config object as a Prop
  let { config = $bindable() } = $props<{ config: ConfigData }>();

  // Local state for this component only
  let isManualWifi = $state(false);
  let scanPromise = $state<Promise<WifiNetwork[]> | null>(null);
  const isDev = import.meta.env.DEV;

  function triggerScan() {
    const endpoint = isDev ? resolve('/api/scan.json' as any) : '/api/scan';
    scanPromise = fetch(endpoint)
      .then(r => r.json())
      .then((data: WifiNetwork[]) => data.sort((a, b) => b.rssi - a.rssi));
  }

  function handleNetworkSelect(event: Event) {
    const select = event.target as HTMLSelectElement;
    if (select.value === '_MANUAL_') {
      isManualWifi = true;
      config.wifiSsid = ''; // Mutating the prop directly works in Svelte 5!
    }
  }
</script>

<section class="space-y-3">
  <h2 class="text-lg font-semibold text-gray-700 border-b pb-1">🏠 Local Wi-Fi</h2>
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
          <button onclick={() => isManualWifi = false} class="bg-red-100 text-red-700 px-3 py-2 rounded-md">✕</button>
        {:else}
          <select 
            bind:value={config.wifiSsid} 
            onchange={handleNetworkSelect}
            class={`${INPUT_CLASS} bg-white`}
          >
            <option value="" disabled>Select a network...</option>
            {#if scanPromise}
              {#await scanPromise}
                <option disabled>Scanning...</option>
              {:then networks}
                {#each networks as net}
                  <option value={net.ssid}>{net.ssid} ({net.rssi}dBm)</option>
                {/each}
              {/await}
            {/if}
            <option value="_MANUAL_" class="font-bold text-blue-600 bg-blue-50">➕ Enter Manually...</option>
          </select>
          <button onclick={triggerScan} class="bg-gray-200 hover:bg-gray-300 px-3 py-2 rounded-md">🔍</button>
        {/if}
      </div>
    </label>

    <label class="block">
      <span class="text-gray-700 text-sm font-bold">Wi-Fi Password</span>
      <input type="password" bind:value={config.wifiPassword} class="mt-1 block w-full rounded-md border-gray-300 shadow-sm border p-2" />
    </label>
  </div>
</section>