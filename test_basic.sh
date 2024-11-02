#!/bin/bash
# Базовый тест - проверка нормальной работы системы

echo "=== Запуск базового теста ==="
docker-compose up -d
sleep 30

echo "=== Проверка логов ==="
docker-compose logs

echo "=== Завершение теста ==="
docker-compose down