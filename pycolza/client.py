import _colza_client


ColzaCommunicator = _colza_client.ColzaCommunicator


class ColzaClient(_colza_client.ColzaClient):

    def __init__(self, engine):
        super().__init__(engine.get_internal_mid())

    def make_distributed_pipeline_handle(self, *args, **kwargs):
        return super().make_distributed_pipeline_handle(*args, **kwargs)
