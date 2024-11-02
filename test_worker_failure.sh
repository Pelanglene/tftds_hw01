#!/bin/bash
# Тест отказа воркера

echo "=== Запуск системы ==="
docker-compose up -d
# sleep 2

echo "=== Имитация падения worker2 ==="
docker-compose stop worker2
sleep 40

echo "=== Завершение теста ==="
docker-compose down 
