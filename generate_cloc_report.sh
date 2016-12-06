#!/bin/bash
cloc --exclude-dir=doc,build --exclude-ext=d,opendb,db --exclude-lang=make --report-file=cloc_report.txt .
unix2dos cloc_report.txt
