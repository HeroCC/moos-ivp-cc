#!/bin/bash

echo "Starting all in background..."

pAntler alpha.moos &
pAntler bravo.moos &
pAntler shoreside.moos &

wait
