FROM moosivp/moos-ivp:r10032-gui as cc_builddeps

USER root
RUN apt-get -y update && DEBIAN_FRONTEND=noninteractive apt-get install -y libssl-dev \
    libboost-system-dev libboost-thread-dev libboost-coroutine-dev libboost-context-dev \
    libsndfile-dev libopus-dev portaudio19-dev \
    libprotobuf-dev protobuf-compiler liblog4cpp5-dev && apt-get -y clean
USER moos

ENV MOOS="moos-ivp-pavlab"
ENV PATH="/home/moos/${MOOS}/bin:${PATH}"
ENV IVP_BEHAVIOR_DIRS="/home/moos/${MOOS}/lib:${IVP_BEHAVIOR_DIRS}"

RUN svn co -r120 https://oceanai.mit.edu/svn/moos-ivp-pavlab-aro moos-ivp-pavlab
RUN cd "${HOME}/moos-ivp-pavlab" && ./build.sh

FROM cc_builddeps
LABEL maintainer = Conlan Cesar <conlanc@csail.mit.edu>

ENV MOOS="moos-ivp-cc"
ENV PATH="/home/moos/${MOOS}/bin:${PATH}"
ENV IVP_BEHAVIOR_DIRS="/home/moos/${MOOS}/lib:${IVP_BEHAVIOR_DIRS}"

ENV GUI="no"

CMD [ "bash", "-c", "${HOME}/${MOOS}/docker-entrypoint.sh" ]

USER root
RUN apt-get -y update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    iputils-ping \
    && apt-get -y clean
USER moos

COPY --chown=moos:moos "." "/home/moos/${MOOS}"

RUN cd "${HOME}/${MOOS}" && ./build.sh

