#!/usr/bin/env python

import serial
import pika
import sys
import time
import datetime

if len(sys.argv) > 1:
  time.sleep(float(sys.argv[1]))

s = serial.Serial('/dev/ttyAMA0', baudrate = 38400)

connection = pika.BlockingConnection(pika.ConnectionParameters(
    'vsol.commology.org',
    5672,
    '/',
    pika.PlainCredentials('kavlar', 'commology'),
    socket_timeout = 30
))

channel = connection.channel()

channel.exchange_declare(
    exchange = 'zig',
    type = 'topic',
    durable = True
)

while True:
    str = s.readline()
    message_body = str
    [dev, seq, rh, t] = str.rstrip().split(',')

    timestr = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime())

    print timestr, ':', dev, ':', seq, ':', seq, ':', rh, ':', t

    header = {
        'device' : 'DHT',
        'SEQ' : seq,
        'datetime' : timestr,
        'RH' : rh,
        'T' : t
    }
    prop = pika.BasicProperties(
        delivery_mode = 2,
        expiration = '60000',
        content_type = 'test/plain; charset=utf-8',
        headers = header
    )
    channel.basic_publish(
        exchange = 'zig',
        routing_key = 'IoT.DHT',
        properties = prop,
        body = message_body
    )

wiringpi.serialClose(serial)

