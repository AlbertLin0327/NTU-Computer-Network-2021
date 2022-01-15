import axios from "axios";
import IUser from './User';

const hostname = process.env.REACT_APP_BACKEND_HOSTNAME;

export class NetworkServices {
  static Login = async (name: string): Promise<string[] | undefined> => {
    const endpoint = new URL(`/user/${name}`, hostname).href;
    const response = axios.get(endpoint);
    return response
      .then((res) => {
        return res.data;
      })
      .catch((err) => {
        console.error(err);
        throw err.response;
      });
  };

  static Addfriend = async (name: string, data: string) => {
    const endpoint = new URL(`/user/${name}`, hostname).href;
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
}