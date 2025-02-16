#-----------------------------------------
# 		Legal
#-----------------------------------------

PYTHON:=python
LEGAL_NOTICE_PYTHON_SCRIPT:=source/legal/legal_notice.py

.PHONY: legal

legal:
	$(PYTHON) $(LEGAL_NOTICE_PYTHON_SCRIPT)
