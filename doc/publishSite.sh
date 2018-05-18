#!/bin/sh
hugo
rsync -avzh -e ssh --progress --delete ./public/ root@ingenuity.io:/home/ingescape/www/
scp htaccess root@ingenuity.io:/home/ingescape/www/.htaccess
ssh root@ingenuity.io 'chown -R ingescape.users /home/ingescape/www'
rm -Rf public

