#!/bin/bash/

# arguments = iwyu files

iwyu -I./list/include/ -I./common/include/ $@
