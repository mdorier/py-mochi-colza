import _colza_client


class ColzaClient(_colza_client.ColzaClient):

    def __init__(self, engine):
        super().__init__(engine.get_internal_mid())
