# *******************************************************************************
#
# package:     Log4Qt
# file:        log4qt.pri
# created:     September 2007
# author:      Martin Heinrich
#
# 
# Copyright 2007 Martin Heinrich
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
# *******************************************************************************

INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..
HEADERS += \
    $$PWD/appender.h \
    $$PWD/appenderskeleton.h \
    $$PWD/basicconfigurator.h \
    $$PWD/consoleappender.h \
    $$PWD/dailyrollingfileappender.h \
    $$PWD/fileappender.h \
    $$PWD/helpers/classlogger.h \
    $$PWD/helpers/configuratorhelper.h \
    $$PWD/helpers/datetime.h \
    $$PWD/helpers/factory.h \
    $$PWD/helpers/initialisationhelper.h \
    $$PWD/helpers/logerror.h \
    $$PWD/helpers/logobject.h \
    $$PWD/helpers/logobjectptr.h \
    $$PWD/helpers/optionconverter.h \
    $$PWD/helpers/patternformatter.h \
    $$PWD/helpers/properties.h \
    $$PWD/hierarchy.h \
    $$PWD/layout.h \
    $$PWD/level.h \
    $$PWD/log4qt.h \
    $$PWD/logger.h \
    $$PWD/loggerrepository.h \
    $$PWD/loggingevent.h \
    $$PWD/logmanager.h \
    $$PWD/mdc.h \
    $$PWD/ndc.h \
    $$PWD/patternlayout.h \
    $$PWD/propertyconfigurator.h \
    $$PWD/rollingfileappender.h \
    $$PWD/simplelayout.h \
    $$PWD/spi/filter.h \
    $$PWD/ttcclayout.h \
    $$PWD/writerappender.h \
    $$PWD/varia/debugappender.h \
    $$PWD/varia/denyallfilter.h \
    $$PWD/varia/nullappender.h \
    $$PWD/varia/levelmatchfilter.h \
    $$PWD/varia/levelrangefilter.h \
    $$PWD/varia/listappender.h \
    $$PWD/varia/stringmatchfilter.h
    
SOURCES += \
    $$PWD/appenderskeleton.cc \
    $$PWD/basicconfigurator.cc \
    $$PWD/consoleappender.cc \
    $$PWD/dailyrollingfileappender.cc \
    $$PWD/fileappender.cc \
    $$PWD/helpers/classlogger.cc \
    $$PWD/helpers/configuratorhelper.cc \
    $$PWD/helpers/datetime.cc \
    $$PWD/helpers/factory.cc \
    $$PWD/helpers/initialisationhelper.cc \
    $$PWD/helpers/logerror.cc \
    $$PWD/helpers/logobject.cc \
    $$PWD/helpers/logobjectptr.cc \
    $$PWD/helpers/optionconverter.cc \
    $$PWD/helpers/patternformatter.cc \
    $$PWD/helpers/properties.cc \
    $$PWD/hierarchy.cc \    
    $$PWD/layout.cc \
    $$PWD/level.cc \
    $$PWD/log4qt.cc \
    $$PWD/logger.cc \
    $$PWD/loggerrepository.cc \
    $$PWD/loggingevent.cc \
    $$PWD/logmanager.cc \
    $$PWD/mdc.cc \
    $$PWD/ndc.cc \
    $$PWD/patternlayout.cc \
    $$PWD/propertyconfigurator.cc \
    $$PWD/rollingfileappender.cc \
    $$PWD/simplelayout.cc \
    $$PWD/spi/filter.cc \
    $$PWD/ttcclayout.cc \    
    $$PWD/writerappender.cc \
    $$PWD/varia/debugappender.cc \
    $$PWD/varia/denyallfilter.cc \
    $$PWD/varia/nullappender.cc \
    $$PWD/varia/levelmatchfilter.cc \
    $$PWD/varia/levelrangefilter.cc \
    $$PWD/varia/listappender.cc \
    $$PWD/varia/stringmatchfilter.cc
    

OTHER_FILES +=