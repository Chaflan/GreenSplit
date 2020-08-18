TEMPLATE = subdirs

SUBDIRS += \
    BillSplit-Core \
    BillSplit-Desktop

BillSplit-Desktop.depends = BillSplit-Core
