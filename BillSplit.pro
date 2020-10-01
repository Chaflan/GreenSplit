TEMPLATE = subdirs

SUBDIRS += \
    BillSplit-Core \
    BillSplit-Desktop \
    BillSplit-Mobile

BillSplit-Desktop.depends = BillSplit-Core
BillSplit-Mobile.depends = BillSplit-Core
