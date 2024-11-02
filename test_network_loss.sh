#!/bin/bash
# Тест потери пакетов

echo "=== Запуск системы ==="
docker-compose up -d
sleep 10

echo "=== Имитация потери пакетов для worker3 (100%) ==="
docker-compose exec worker2 iptables -A INPUT -m statistic --mode random --probability 1 -j DROP
sleep 15

echo "=== Проверка логов ==="
docker-compose logs

echo "=== Завершение теста ==="
docker-compose down
