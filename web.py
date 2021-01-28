from flask import Flask, jsonify, request

app = Flask(__name__)


@app.route('/HelloWorld')
def hello_world():
    return "Hello World!"

@app.route("/api/v1.0/machine/deployment_status", methods=["PUT"])
def update_machine_deployment_status():
    """
    http://ip:port/api/v1.0/machine/deployment_status
    body={}
    headers={}
    :param:
    :return: body.update({})
    """
    import ipdb;ipdb.set_trace()

    #print(request.get_json())
    s = request.get_data()
    print(s)
    if isinstance(s, bytes):
        status_info = {}
        s = s.decode().split(',')
        print(s)
        for i in s:
            print(i)
            status_info[i.split("=")[0]]= i.split("=")[1]
    else:
        status_info = request.get_json(force=True)

    logger.info(status_info)


    status_info = request.get_json(force=True)
    db_res = db.update_machine_deployment_status(status_info)
    if not db_res["code"] and not db_res["res"]:
        logger.info("Update Machine Deployment status with id %s into db" % db_res["id"])
    return jsonify(db_res)



if __name__ == "__main__":
    app.run(debug=True)
