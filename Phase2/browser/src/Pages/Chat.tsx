import React from "react";
import { Navigate } from "react-router-dom";
import { Container, Card, Form, Row, Col, Button, Table } from 'react-bootstrap';
import { NetworkServices } from "./MessageService";
import { setCookie, getCookie } from "../Utils/cookie";

interface LoginPageState {
  Sender: string | undefined;
  Receiver: string | undefined;
  redirect: string | undefined;
  SendMessage: string | undefined;
  AllMessage: Message[] | undefined;
}

type Message = {
    Id: number;
    Sender: string;
    Receiver: string;
    type: number;
    Content: string;
}

class Chat extends React.Component<{}, LoginPageState> {
  constructor(props: LoginPageState) {
    super(props);
    this.state = { Sender: undefined, Receiver: undefined, SendMessage: undefined, redirect: undefined, AllMessage: [] };
  }

  componentDidMount = async () => {
    await this.updatedata();
  }

  updatedata = async () => {
    var Pathname = window.location.pathname;
    var SPname = Pathname.split('/');
    const Sender = SPname[2];
    const Receiver = SPname[3];
    if(Sender && Receiver){
        var messages = await NetworkServices.GetMessage(Sender, Receiver);
        if(messages){
            var messagesList: string[] = messages.replace('[','').replace(']','').slice(0, -1).split(',');
            var AllMessage: Message[] = [];
            messagesList.map((message) => {
                var newM: Message = {
                    Id: -1,
                    Sender: "",
                    Receiver: "",
                    type: 0,
                    Content: ""
                };
                var message_t = message.split('_');
                newM.Id = Number(message_t[0]);
                newM.Sender = message_t[1];
                newM.Receiver = message_t[2];
                newM.type = Number(message_t[3]);
                newM.Content = message_t[4];
                AllMessage.push(newM);
            })
            this.setState({AllMessage: AllMessage});
        }
        this.setState({Sender, Receiver});
    }
    
  } 
  
  handleChange = (event: any) => {
    this.setState({SendMessage: event.target.value});
  }

  onSubmit = async () => {
    if(this.state.SendMessage && this.state.Sender && this.state.Receiver){
        try {
            await NetworkServices.SendMessage(this.state.Sender, this.state.Receiver, this.state.SendMessage);
        } catch (e) {
            console.log(e);
        }
        await this.updatedata();
    }
  }


  render(){
    const { Sender, Receiver, redirect, SendMessage, AllMessage } = this.state;
    if (redirect) {
      return <Navigate to={redirect} />;
    }
    return (
      <>
        <Container fluid>
          <Row>
            <h2 className="mx-2 my-2">Hello, {Sender}, welcome you to your chat room with {Receiver}</h2>
          </Row>
          <Row>
          <Table className="table-hover table-striped">
            <thead>
              Chat Histroy
            </thead>
            <tbody>
            
            {AllMessage && AllMessage.map((message) => {
              if(message.Sender === Sender){
                return (
                  <tr key={message.Id}>
                    <td></td>
                    <td>{message.Content}</td>
                </tr>
                );
              }else{
                return (
                    <tr key={message.Id}>
                        <td>{message.Content}</td>
                      <td></td>
                  </tr>
                  );
              }})}
            </tbody>
          </Table>
        </Row>
        <Row>
            <Col>
                <Form>
                <Form.Group controlId="exampleForm.ControlInput1">
                    <Form.Control 
                    type="string" 
                    placeholder="Message"
                    onChange={(e) => this.handleChange(e)}
                    />
                </Form.Group>
                </Form>
            </Col>
            <Col> 
                <Button variant="outline-primary" onClick={() => this.onSubmit()}>Send</Button>
            </Col>
        </Row>
        </Container>
      </>
    );
  }
  
}

export default Chat;
