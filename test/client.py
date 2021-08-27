import sys
sys.path.append('../build/lib.linux-x86_64-3.8')

import pymargo
from pymargo.core import Engine
from pycolza.client import ColzaClient, ColzaCommunicator, Type
import pycolza.client
import pyssg

from mpi4py import MPI

import numpy as np

def run_test(engine):
    print('Creating communicator')
    comm = ColzaCommunicator(MPI.COMM_WORLD)
    print('Creating client')
    client = ColzaClient(engine)
    print('Creating pipeline handle')
    pipeline = client.make_distributed_pipeline_handle(
        comm=comm, ssg_file='group.ssg',
        provider_id=0, name='test_pipeline',
        check=True)
    for i in range(0, 10):
        print('Starting iteration '+str(i))
        pipeline.start(i)
        print('Sending numpy array')
        data = np.random.rand(10, 20, 30)
        print('Data sums to '+str(np.sum(data)))
        pipeline.stage(dataset_name='my_dataset', iteration=i,
                       block_id=0, type=Type.FLOAT64, data=data)
        print('Executing pipeline')
        pipeline.execute(iteration=i, auto_cleanup=True)
    print('Done')

if __name__ == '__main__':
    with Engine('na+sm', pymargo.server) as engine:
        pyssg.init()
        run_test(engine)
        engine.finalize()
        pyssg.finalize()
