import {getBabelOutputPlugin} from '@rollup/plugin-babel';
import * as path from 'path';
import {defineConfig} from 'vite';
import dts from 'vite-plugin-dts';
import {viteStaticCopy} from 'vite-plugin-static-copy';

const copyPlugin = viteStaticCopy({
    targets: [{
        src: `dist/index.js`,
        dest: `${path.resolve(__dirname, '../../../lib/az_js/')}`,
        rename: 'mathjax_src.cpp',
        transform: (content: string) => {
            console.log(`content.length=${content.length}`)
            // (16*1024-4)/2, '/2' for utf8
            const chunks = content.match(/(.|[\r\n]){1,8190}/g);
            console.log(`chunks.length=${chunks?.length}`)
            let binBuffer = '';
            for (let i = 0; i < chunks.length; i++) {
                binBuffer = `${binBuffer} R"xxxxx(${chunks![i]})xxxxx"`;
            }
            return String.raw`
namespace az::js::impl::mathjax_src {
const char* CODE_BUFFER = ${binBuffer};
}
`;
        }
    }]
});
// https://vitejs.dev/config/
export default defineConfig({
    plugins: [
        dts(),
        getBabelOutputPlugin({presets: ['@babel/preset-env',]}),
        copyPlugin,
    ],
    build: {
        minify: 'esbuild',
        target: ['es6'],
        lib: {
            entry: path.resolve(__dirname, 'index.ts'),
            formats: ['es'],
            fileName: 'index'
        },
        rollupOptions: {
            // https://rollupjs.org/configuration-options/
        },
    },
    define: {
        'process': {}
    }
});
