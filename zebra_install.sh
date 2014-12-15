#!/bin/bash
HOST="http://119.167.216.80/zebra"
wget $HOST/zebra.tar.gz

tar zxvf zebra.tar.gz
cd zebra
mkdir -p /usr/local/zebra/modules
mkdir -p /etc/zebra
#copy modules files
\cp modules/*.so /usr/local/zebra/modules
#copy bin file
\cp bin/zebra /usr/bin/zebra
chmod 777 /usr/bin/zebra
#copy configure files
\cp conf/zebra.conf /etc/zebra/zebra.conf
\cp conf/zebra.logrotate /etc/logrotate.d/zebra
\cp conf/zebra.cron /etc/cron.d/zebra

cd ..
rm -rf zebra
rm -rf zebra.tar.gz

echo "zebra install over!"
