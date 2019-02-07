#!/usr/bin/python
# -*- coding: utf-8 -*-
import json

def teste():
    return json.dumps(['foo1111', {'bar': ('baz', None, 1.0, 2)}])


def application(environ, start_response):
    headers = [('Content-Type', 'application/json')]
    start_response('200 OK', headers)
    test = json.dumps(environ['PATH_INFO'] + environ['REQUEST_METHOD'])
    test1 = test.encode('utf-8')      
    return [test1]



