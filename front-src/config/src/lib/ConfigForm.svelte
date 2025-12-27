<script lang="ts">
  import { onMount } from 'svelte';
  import { resolve } from '$app/paths';
  import type { ConfigData } from '$lib/types';
  import { INPUT_CLASS } from '$lib/styles';
  
  // Import the new Sub-Components
  import WifiSection from '$lib/sections/WifiSection.svelte';
  import TimeSection from '$lib/sections/TimeSection.svelte';

  // State
  let config = $state<ConfigData>({
    apSsid: '', apPassword: '',
    wifiSsid: '', wifiPassword: '',
    weatherApiKey: '', weatherCity: '',
    ntpServer: '', timeZone: ''
  });

  let loadPromise = $state<Promise<any> | null>(null);
  let savePromise = $state<Promise<string> | null>(null);
  const isDev = import.meta.env.DEV;

  // Load Logic
  onMount(() => {
    const endpoint = isDev ? resolve('/api/config.json' as any) : '/api/config';
    loadPromise = fetch(endpoint)
      .then(r => r.json())
      .then(data => Object.assign(config, data));
  });

  // Save Logic
  function saveAll() {
    if(isDev) {
        // DEBUG: Print the exact JSON that would be sent to the ESP32
        console.log("----------------------------------------");
        console.log("💾 MOCK SAVE TRIGGERED");
        console.log("Payload:", JSON.stringify(config, null, 2));
        console.log("----------------------------------------");      
        savePromise = new Promise(r => setTimeout(() => r("Saved (Mock)"), 1000));
        return;
    }
    savePromise = fetch('/api/save', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(config)
    }).then(async r => {
      if (!r.ok) throw new Error('Error saving');
      return "Saved! Rebooting...";
    });
  }
</script>

<div class="w-full max-w-2xl bg-white shadow-lg rounded-xl overflow-hidden border border-gray-200">
  <div class="bg-blue-600 p-4">
    <h1 class="text-white text-xl font-bold text-center">System Configuration</h1>
  </div>

  <div class="p-6 space-y-8"> {#if loadPromise}
      {#await loadPromise}
        <div class="text-center text-gray-500 py-10 animate-pulse">Loading...</div>
      {:then _}
        
        <section class="space-y-3">
            <h2 class="text-lg font-semibold text-gray-700 border-b pb-1">📡 SoftAP</h2>
            <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
                <label class="block">
                    <span class="text-gray-700 text-sm font-bold">SSID</span>
                    <input type="text" bind:value={config.apSsid} class={INPUT_CLASS} />
                </label>
                <label class="block">
                    <span class="text-gray-700 text-sm font-bold">Password</span>
                    <input type="password" bind:value={config.apPassword} class={INPUT_CLASS} />
                </label>
            </div>
        </section>

        <WifiSection bind:config />

        <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
            <section class="space-y-3">
                <h2 class="text-lg font-semibold text-gray-700 border-b pb-1">⛅ Weather</h2>
                <label class="block">
                    <span class="text-gray-700 text-sm font-bold">API Key</span>
                    <input type="text" bind:value={config.weatherApiKey} class={`${INPUT_CLASS} font-mono`} />
                </label>
                <label class="block">
                    <span class="text-gray-700 text-sm font-bold">City</span>
                    <input type="text" bind:value={config.weatherCity} class={INPUT_CLASS} />
                </label>
            </section>

            <TimeSection bind:config />
        </div>

        <div class="pt-4">
          <button onclick={saveAll} class="w-full bg-blue-600 hover:bg-blue-700 text-white font-bold py-3 px-4 rounded shadow transition transform active:scale-95">
            💾 Save Configuration
          </button>
        </div>
        
        {#if savePromise}
          {#await savePromise}
            <div class="mt-4 p-3 bg-blue-100 text-blue-800 rounded text-center">Saving...</div>
          {:then msg}
            <div class="mt-4 p-3 bg-green-100 text-green-800 rounded text-center">{msg}</div>
          {/await}
        {/if}

      {:catch error}
        <div class="text-red-500 text-center">Connection Error: {error.message}</div>
      {/await}
    {/if}
  </div>
</div>
