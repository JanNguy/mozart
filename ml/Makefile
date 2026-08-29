VERSION	=	v1
PYTHON	?=	python3.10
VENV	?=	.venv
SSL_CERT_FILE	?=	/etc/ssl/cert.pem

.PHONY: all venv train

all:

venv:
	$(PYTHON) -m venv $(VENV)
	PIP_CERT=$(SSL_CERT_FILE) $(VENV)/bin/python -m pip install --upgrade pip --use-deprecated=legacy-certs
	PIP_CERT=$(SSL_CERT_FILE) $(VENV)/bin/python -m pip install -r requirements.txt --use-deprecated=legacy-certs

train:
	$(VENV)/bin/python model.py