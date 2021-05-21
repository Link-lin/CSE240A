cnf ?= .env
include  $(cnf)
export $(shell sed 's/=.*//' $(cnf))

default: 
	@echo 'choose a target'


build: 
	sudo docker-compose build

run: 
	@test -s ${src} || { echo "${htdocs} does not exist"; exit 1; }
	@test -s ${trace} || { echo "${etc} does not exist"; exit 1; }
	sudo docker-compose up

clean:
	sudo docker-compose down
.PHONY: all test clean run
