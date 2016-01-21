#!/bin/env python

import pika
import redis
import datetime

EXPIRATION = 5 * 60 

connection = pika.BlockingConnection(pika.ConnectionParameters(
    'vsol.commology.org',
    5672,
    '/',
    pika.PlainCredentials('kavlar', 'commology'),
    socket_timeout = 60
))

channel = connection.channel()

channel.queue_declare(
    queue = 'zag',
    durable = True)

rdb = redis.StrictRedis(
    host = 'localhost',
    port = 6379,
    db = 2
)

def procField(field, header, body):
    if 'device' in header.keys() and 'datetime' in header.keys():
        timestamp = datetime.datetime.strptime(header['datetime'], '%Y-%m-%d %H:%M:%S')
        epoch = timestamp.strftime('%s')
        if field in header.keys():
            rkey = epoch + ':' + field
            rdb.setex(rkey, EXPIRATION, header[field])
        else:
            print "No valid %r field" % field
    else:
        print "Unrecognized message header"

def onMessage(ch, method, properties, body):
    now = datetime.datetime.now()
    timestamp = str(now)
    print "%r [x] %r" % (timestamp, properties)
    header = properties.headers
    procField('RH', header, body)
    procField('T', header, body)

channel.basic_consume(
    onMessage,
    queue = 'zag',
    no_ack = True)

channel.start_consuming()

