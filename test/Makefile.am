AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/bindings/polls

noinst_PROGRAMS  = metaquery eris-connect

check_PROGRAMS = Entity_unittest Account_unittest \
                 Avatar_unittest BaseConnection_unittest Calendar_unittest \
                 Connection_unittest DeleteLater_unittest Entity_unittest \
                 EntityRef_unittest EntityRouter_unittest Exceptions_unittest \
                 Factory_unittest IGRouter_unittest Lobby_unittest \
                 Log_unittest LogStream_unittest MetaQuery_unittest \
                 Metaserver_unittest Operations_unittest Person_unittest \
                 Poll_unittest Redispatch_unittest \
                 Response_unittest Room_unittest Router_unittest \
                 ServerInfo_unittest Task_unittest EventService_unittest \
                 TypeBoundRedispatch_unittest \
                 TransferInfo_unittest \
                 TypeInfo_unittest TypeService_unittest Types_unittest \
                 View_unittest TerrainMod_unittest \
                 Metaserver_integrationtest PollWinsock_unittest \
                 Account_integrationtest
TESTS = $(check_PROGRAMS)
RECHECK_LOGS = testEris.log

EXTRA_PROGRAMS = testEris

testEris_SOURCES = tests.cpp \
    stubServer.h stubServer.cpp \
    clientConnection.cpp clientConnection.h \
    agent.cpp agent.h \
    commander.cpp commander.h \
    controller.cpp controller.h \
    testUtils.h testUtils.cpp \
    objectSummary.h \
    netTests.cpp netTests.h \
    setupHelpers.cpp setupHelpers.h \
    signalHelpers.h \
    testOutOfGame.cpp testOutOfGame.h \
    viewTest.cpp viewTest.h \
    avatarTest.cpp avatarTest.h \
    calendarTest.cpp calendarTest.h

LDADD = $(top_builddir)/Eris/liberis-1.4.la $(ERIS_DEPS)

metaquery_SOURCES = metaQuery.cpp

eris_connect_SOURCES = connect.cpp

Account_unittest_SOURCES = Account_unittest.cpp
Account_unittest_LDADD = \
    $(top_builddir)/Eris/Account.lo \
    $(top_builddir)/Eris/Exceptions.lo \
    $(ERIS_DEPS)

Avatar_unittest_SOURCES = Avatar_unittest.cpp SignalFlagger.h
BaseConnection_unittest_SOURCES = BaseConnection_unittest.cpp SignalFlagger.h

Calendar_unittest_SOURCES = Calendar_unittest.cpp
Calendar_unittest_LDADD = \
    $(top_builddir)/Eris/Calendar.lo \
    $(top_builddir)/Eris/Exceptions.lo \
    $(ERIS_DEPS)

Connection_unittest_SOURCES = Connection_unittest.cpp
DeleteLater_unittest_SOURCES = DeleteLater_unittest.cpp
EntityRef_unittest_SOURCES = EntityRef_unittest.cpp
EntityRouter_unittest_SOURCES = EntityRouter_unittest.cpp

Entity_unittest_SOURCES = Entity_unittest.cpp
Entity_unittest_LDADD = \
    $(top_builddir)/Eris/Entity.lo \
    $(ERIS_DEPS)

Exceptions_unittest_SOURCES = Exceptions_unittest.cpp
Factory_unittest_SOURCES = Factory_unittest.cpp

IGRouter_unittest_SOURCES = IGRouter_unittest.cpp SignalFlagger.h
IGRouter_unittest_LDADD = \
    $(top_builddir)/Eris/IGRouter.lo \
    $(top_builddir)/Eris/Response.lo \
    $(ERIS_DEPS)

Lobby_unittest_SOURCES = Lobby_unittest.cpp
LogStream_unittest_SOURCES = LogStream_unittest.cpp
Log_unittest_SOURCES = Log_unittest.cpp
MetaQuery_unittest_SOURCES = MetaQuery_unittest.cpp

Metaserver_unittest_SOURCES = Metaserver_unittest.cpp
Metaserver_unittest_LDADD = \
    ${top_builddir}/Eris/Metaserver.lo \
    $(ERIS_DEPS)

Operations_unittest_SOURCES = Operations_unittest.cpp
Person_unittest_SOURCES = Person_unittest.cpp
Poll_unittest_SOURCES = Poll_unittest.cpp
Redispatch_unittest_SOURCES = Redispatch_unittest.cpp
Response_unittest_SOURCES = Response_unittest.cpp
Room_unittest_SOURCES = Room_unittest.cpp
Router_unittest_SOURCES = Router_unittest.cpp
ServerInfo_unittest_SOURCES = ServerInfo_unittest.cpp ElementExerciser.h
Task_unittest_SOURCES = Task_unittest.cpp

EventService_unittest_SOURCES = EventService_unittest.cpp
EventService_unittest_LDADD = \
    ${top_builddir}/Eris/EventService.lo \
    $(ERIS_DEPS)

TransferInfo_unittest_SOURCES = TransferInfo_unittest.cpp
TransferInfo_unittest_LDADD = \
    ${top_builddir}/Eris/TransferInfo.lo

TypeBoundRedispatch_unittest_SOURCES = TypeBoundRedispatch_unittest.cpp
TypeInfo_unittest_SOURCES = TypeInfo_unittest.cpp
TypeService_unittest_SOURCES = TypeService_unittest.cpp
Types_unittest_SOURCES = Types_unittest.cpp
View_unittest_SOURCES = View_unittest.cpp
TerrainMod_unittest_SOURCES = TerrainMod_unittest.cpp

Metaserver_integrationtest_SOURCES = Metaserver_integrationtest.cpp
Metaserver_integrationtest_LDADD = \
    ${top_builddir}/Eris/Metaserver.lo \
    ${top_builddir}/Eris/MetaQuery.lo $(ERIS_DEPS)

PollWinsock_unittest_SOURCES = PollWinsock_unittest.cpp
PollWinsock_unittest_LDADD =

Account_integrationtest_SOURCES = Account_integrationtest.cpp SignalFlagger.h
