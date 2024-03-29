# O que precisa? Ter uma maquina com OpenGL instalado e a lib GLFW

# Maneira mais facil para quem está do zero:

Obs.: Nao tente fazer isso no windows, é muita dor de cabeça. Pelo linux é simplesmente mais facil. Ou utilize o WSL 2.

```
1 - instala o ubuntu LTS ( ou qualquer SO linux )
3 - Abre o terminal, sudo su
4 - Instalar o GLFW: apt-get install libglfw3-dev
5 - Instalar a opengl: apt-get install freeglut3-dev
```

# Pra rodar:
(caso nao tenha o g++ rodar (em super usuario): apt install g++)
```
1 - Compila: 
    g++ -o ueize ./main.cpp -lglut -lGLU -lGL -lm -lglfw
    
2 - Na mesma pasta do main.cpp será gerado um exec com titulo ueize. clique duas vezes e voala.
```

# CASO SEJA UM DEV
recomendo usar o VSCODE com as seguintes configs:

Plugins para baixar:
```
ms-vscode.cpptools
ms-vscode.cpptools-extension-pack
ms-vscode.cpptools-themes
```
```
1 - Abra o arquivo main.cpp, aperte F5.
2 - Selecione um dos COMPILADORES que o VSCode vai te mostrar
3 - Ao escolher, será gerado na raiz do seu projeto uma pasta .vscode e sua execucao tera FALHADO
4 - Copie e cole o json abaixo no /vscode/tasks.json:

{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: g++ arquivo de build ativo",
            "command": "/usr/bin/g++",
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}/ueize",
                "-lglut",
                "-lGLU",
                "-lGL",
                "-lm",
                "-lglfw",
            ],
            "options": {
                "cwd": "${fileDirname}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "Tarefa gerada pelo Depurador."
        }
    ],
    "version": "2.0.0"
}

5 - Aperta F5 denovo, e pronto.
```

# Lembretes:
F9 para setar os breakpoints e é isso. 
