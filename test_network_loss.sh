#!/bin/bash
# Тест потери пакетов

echo "=== Запуск системы ==="
docker-compose up -d
sleep 10

echo "=== Имитация потери пакетов для worker3 (50%) ==="
docker-compose exec worker3 tc qdisc add dev eth0 root netem loss 50%
sleep 15

echo "=== Проверка логов ==="
docker-compose logs

echo "=== Завершение теста ==="
docker-compose down 