import sys
sys.path.append('../build/lib.linux-x86_64-3.8')

import pymargo
from pymargo.core import Engine
from pycolza.client import ColzaClient, ColzaCommunicator

from mpi4py import MPI

import numpy as np

def run_test(engine):
    comm = ColzaCommunicator(MPI.COMM_WORLD)
    client = ColzaClient(engine)
    client.make_distributed_pipeline_handle(
        comm=comm, ssg_file='group.ssg',
        provider_id=0, name='test_pipeline',
        check=True)

if __name__ == '__main__':
    with Engine('ofi+tcp', pymargo.server) as engine:
        run_test(engine)
        engine.finalize()
