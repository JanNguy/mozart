VERSION	=	v1
PYTHON	?=	python3.10
VENV	?=	.venv
SSL_CERT_FILE	?=	/etc/ssl/cert.pem
REQUIREMENTS	:=	$(abspath $(dir $(lastword $(MAKEFILE_LIST)))../requirements.txt)

.PHONY: all venv train

all:

venv: $(VENV)/bin/python
	@echo "Environnement virtuel pret: $(VENV)"

$(VENV)/bin/python:
	$(PYTHON) -m venv $(VENV)
	PIP_CERT=$(SSL_CERT_FILE) $(VENV)/bin/python -m pip install --upgrade pip --use-deprecated=legacy-certs
	PIP_CERT=$(SSL_CERT_FILE) $(VENV)/bin/python -m pip install -r $(REQUIREMENTS) --use-deprecated=legacy-certs

train: venv
	$(VENV)/bin/python model.py