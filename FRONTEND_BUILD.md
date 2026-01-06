# Front-end Build Integration

## Visão Geral

O front-end da aplicação Weather Ticker é construído automaticamente durante o processo de build do ESP-IDF. Ele usa Svelte + Vite e é integrado ao firmware através do sistema de partições LittleFS.

## Estrutura

```
weather-ticker/
├── front-src/config/          # Código-fonte do front-end (Svelte)
│   ├── src/
│   ├── package.json
│   └── vite.config.ts
├── front/config/              # Build output (gerado automaticamente)
├── build-frontend.sh          # Script de build do front-end
└── CMakeLists.txt            # Configuração integrada
```

## Processo de Build

Durante o `idf.py build`, o seguinte acontece:

1. **CMake detecta** que o target `littlefs_littlefs_bin` precisa ser construído
2. **Dependência acionada**: `build_frontend` target é executado primeiro
3. **Script executa**:
   - Verifica se npm está instalado
   - Instala dependências (`npm install`) se necessário
   - Executa `npm run deploy` que:
     - Compila o front-end com Vite
     - Copia o output para `front/config/`
4. **LittleFS cria** a imagem da partição com o conteúdo de `front/`
5. **Imagem incluída** no flash do ESP32

## Desenvolvimento Local

### Build manual do front-end

```bash
cd front-src/config
npm install
npm run dev         # Servidor de desenvolvimento
npm run build       # Build de produção
npm run deploy      # Build + cópia para front/config/
```

### Build completo

```bash
idf.py build        # Constrói tudo (back-end + front-end)
```

## DevContainer

O devcontainer foi configurado para incluir Node.js 20.x:

```dockerfile
# Install Node.js and npm for front-end build
RUN apt-get update -y && \
    apt-get install -y udev curl && \
    curl -fsSL https://deb.nodesource.com/setup_20.x | bash - && \
    apt-get install -y nodejs
```

**Nota**: Se você atualizou o Dockerfile, precisará reconstruir o container:
- VS Code: `Dev Containers: Rebuild Container`
- CLI: `docker-compose up --build`

## Scripts Disponíveis

### build-frontend.sh

Script auxiliar que gerencia o build do front-end:

```bash
./build-frontend.sh <frontend-dir> <output-dir>
```

**Funcionalidades**:
- ✓ Verifica disponibilidade do npm
- ✓ Instala dependências automaticamente
- ✓ Executa o build
- ✓ Valida output
- ✓ Fornece feedback detalhado

## Troubleshooting

### Erro: "npm is not installed"

**Solução**: Reconstrua o devcontainer para instalar o Node.js

### Erro: Build do front-end falha

**Debug**:
```bash
cd front-src/config
npm install --verbose
npm run build
```

### Limpar cache de build

```bash
rm -rf front-src/config/node_modules
rm -rf front-src/config/build
rm -rf front/config
idf.py fullclean
idf.py build
```

## Arquivos Ignorados (.gitignore)

O seguinte é ignorado pelo git:
- `front/config/` - Output do build (gerado automaticamente)
- `front-src/config/node_modules/` - Dependências npm
- `front-src/config/build/` - Build intermediário do Vite
