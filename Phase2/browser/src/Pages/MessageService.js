import axios from "axios";

const hostname = process.env.REACT_APP_BACKEND_HOSTNAME;

export class NetworkServices {
  static Login = async (name) => {
    const endpoint = new URL(`/user/${name}`, hostname).href;
    const response = axios.get(endpoint);
    return response
      .then((res) => {
          console.log(res.data);
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
      });
  };

  static Addfriend = async (name, data) => {
    const endpoint = new URL(`/user/add/${name}`, hostname).href;
    const response = axios.post(endpoint, data);
    return response
      .then((res) => {
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
      });
  };

  static Deletefriend = async (name, data) => {
    const endpoint = new URL(`/user/delete/${name}`, hostname).href;
    const response = axios.post(endpoint, data);
    return response
      .then((res) => {
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
      });
  };

  static SendMessage = async (sender, receiver, data) => {
    const endpoint = new URL(`/send/${sender}/${receiver}`, hostname).href;
    const response = axios.post(endpoint, data);
    return response
      .then((res) => {
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
    });
  }

  static GetMessage = async (sender, receiver) => {
    const endpoint = new URL(`/chat/${sender}/${receiver}`, hostname).href;
    const response = axios.get(endpoint);
    return response
      .then((res) => {
          console.log(res.data);
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
      });
  };

  static SendFile = async (sender, receiver, data) => {
    const endpoint = new URL(`/file/${sender}/${receiver}`, hostname).href;
    const response = axios.post(endpoint, data, {
      headers: { 'Content-Type': 'multipart/form-data'  }});
    return response
      .then((res) => {
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
    });
  }

  static SendImage = async (sender, receiver, data) => {
    const endpoint = new URL(`/img/${sender}/${receiver}`, hostname).href;
    const response = axios.post(endpoint, data, {
      headers: { 'Content-Type': 'multipart/form-data'  }});
    return response
      .then((res) => {
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
    });
  }

  static GetImage = async (id) => {
    const endpoint = new URL(`/img/${id}`, hostname).href;
    const response = axios.get(endpoint);
    return response
      .then((res) => {
          console.log(res.data);
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
      });
  };

  static GetFile = async (id) => {
    const endpoint = new URL(`/file/${id}`, hostname).href;
    const response = axios.get(endpoint);
    return response
      .then((res) => {
          console.log(res.data);
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
      });
  };
}