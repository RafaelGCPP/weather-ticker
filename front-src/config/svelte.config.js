import adapter from '@sveltejs/adapter-static'; // <--- Change this from adapter-auto
import { vitePreprocess } from '@sveltejs/vite-plugin-svelte';

/** @type {import('@sveltejs/kit').Config} */
const config = {
	preprocess: vitePreprocess(),

	kit: {
		// adapter-static config
		adapter: adapter({
			// default options are fine, but for SPA we usually want this:
			pages: 'build',
			assets: 'build',
			fallback: 'index.html', // <--- IMPORTANT: This enables SPA mode
			precompress: false,
			strict: true
		}),
		paths: {
            base: '/c'
        }
	}
};

export default config;