<script lang="ts">
  import { onMount } from 'svelte';
  import { resolve } from '$app/paths';
  import { INPUT_CLASS } from '$lib/styles';
  import type { ConfigData } from '$lib/types';

  let { config = $bindable() } = $props<{ config: ConfigData }>();
  
  // Interface for the timezone items
  interface TimezoneItem {
    label: string;
    value: string;
  }

  // State to hold the list
  let timezones = $state<TimezoneItem[]>([]);
  let isLoading = $state(true);

  // Load the JSON on mount
  onMount(async () => {
    try {
      // isDev check isn't strictly needed for static files if using resolve() correctly,
      // but 'resolve' ensures /c/ prefix in dev mode.
      const endpoint = resolve('/data/timezones.json' as any);
      
      const res = await fetch(endpoint);
      if (!res.ok) throw new Error('Failed to load timezones');
      
      timezones = await res.json();
    } catch (e) {
      console.error(e);
      // Fallback in case of error (so the UI isn't broken)
      timezones = [{ label: "Error loading list (Use Manual)", value: "UTC0" }];
    } finally {
      isLoading = false;
    }
  });
</script>

<section class="space-y-3">
  <h2 class="text-lg font-semibold text-gray-700 border-b pb-1">🕒 Time (NTP)</h2>
  
  <label class="block">
    <span class="text-gray-700 text-sm font-bold">NTP Server</span>
    <input type="text" bind:value={config.ntpServer} class={INPUT_CLASS} />
  </label>

  <label class="block">
    <span class="text-gray-700 text-sm font-bold">Timezone</span>
    <select 
      bind:value={config.timeZone} 
      class="{INPUT_CLASS} bg-white disabled:bg-gray-100 disabled:text-gray-400"
      disabled={isLoading}
    >
      {#if isLoading}
        <option>Loading timezones...</option>
      {:else}
        <option value="" disabled>Select a timezone...</option>
        {#each timezones as tz}
          <option value={tz.value}>{tz.label}</option>
        {/each}
      {/if}
    </select>
    
    <div class="mt-1 text-xs text-gray-400 font-mono text-right truncate">
        Current: {config.timeZone || "Not set"}
    </div>
  </label>
</section>