#!/bin/env python

import pika
import time
import datetime
import random

connection = pika.BlockingConnection(pika.ConnectionParameters(
    'vsol.commology.org',
    5672,
    '/',
    pika.PlainCredentials('kavlar', 'commology'),
    socket_timeout = 60
))

channel = connection.channel()

channel.exchange_declare(
    exchange = 'zig',
    type = 'topic',
    durable = True
)

while True:
    t = random.random() * 20.0 + 10.0;
    h = random.random() * 100;
    timestr = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime())
    header = {
        'device' : 'DHT',
        'datetime' : timestr,
        'RH' : ('%.1f' % h),
        'T' : ('%.1f' % t)
    }
    prop = pika.BasicProperties(
        delivery_mode = 2,
        content_type = 'test/plain; charset=utf-8',
        headers = header
    )
    channel.basic_publish(
        exchange = 'zig',
        routing_key = 'IoT',
        properties = prop,
        body = ''
    )
    time.sleep(5)

