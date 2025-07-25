#!/bin/bash

REPO_SPACE=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)/..
REPO_NAME_ORIGIN=$(pwd | grep -o '[^/]*$')

# Текст, который нужно добавить для использования бесплатной лицензии PVS-Studio
text="// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com"

cd ${REPO_SPACE}/${REPO_NAME_ORIGIN}
# Найти все .c, .cpp файлы и добавить текст в их начала
find . -type f -name "*.c" -o -name "*.cpp" | while IFS= read -r file;
do
    # Проверяем, что текст еще не добавлен
    if ! grep -qF "$text" "$file"; then
        # Добавляем текст в начало файла
        {
            echo "$text"
            cat "$file"
        } > temp_file && mv temp_file "$file"
        echo "Добавлено в $file"
    else
        echo "Текст уже присутствует в $file"
    fi
done
